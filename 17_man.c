#include "headers.h"

char *create_http_request(const char *command)
{
    static char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
             "GET /?topic=%s&section=all HTTP/1.1\r\n"
             "Host: man.he.net\r\n"
             "User-Agent: iMan/1.0\r\n"
             "Connection: close\r\n\r\n",
             command);
    return request;
}

void fetch_man_page(const char *command)
{
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char *request;
    ssize_t bytes_received;

    // Create the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("65.19.140.5");

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    request = create_http_request(command);

    // Send the request to the server
    if (send(sockfd, request, strlen(request), 0) < 0)
    {
        perror("Send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    char search_string[1000];
    snprintf(search_string, sizeof(search_string), "No matches for \"%s\"", command);
    int flag_print = 0;
    int flag_invalid_command = 0;

    while ((bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0)
    {

        buffer[bytes_received] = '\0';
        if (strstr(buffer, search_string) != NULL)
            flag_invalid_command = 1;

        for (int i = 0; i < bytes_received; i++)
        {
            if (flag_print == 0)
            {
                if (buffer[i] == '>')
                {
                    flag_print = 1;
                }
            }
            else
            {
                if (buffer[i] == '<')
                {
                    flag_print = 0;
                }
                else
                {
                    printf("%c", buffer[i]);
                }
            }
        }
    }

    if (flag_invalid_command == 1)
    {
        printf(RED);
        printf("ERROR: No matches for \"%s\" command\n", command);
        printf(RESET);
    }

    close(sockfd);
}

void man_page(char *command, char arg[20][4096])
{
    fetch_man_page(arg[0]);
}
