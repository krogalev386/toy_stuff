/* определяет типы данных */
#include <sys/types.h>
/* "Главный" по сокетам */
#include <sys/socket.h>
/* sockaddr_in struct, sin_family, sin_port, in_addr_t, in_port_t, ...*/
#include <netdb.h>
#include <netinet/in.h>

#include <thread>
#include <mutex>
#include <unistd.h>

#include <iostream>
#include <string.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int createListenAndAcceptSock(int & listenSocket,
                               int & acceptSocket,
                               struct addrinfo * servinfo){
    listenSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (listenSocket < 0){
        perror("Error calling socket");
        return 1;
    }

    // Освобождение сокета для привязки
    int yes = 1;
    if (setsockopt(listenSocket, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("setsockopt");
        return 1;
    }
    // Связывание с портом
    int b = bind(listenSocket, servinfo->ai_addr, servinfo->ai_addrlen);
    if (b < 0){
        perror("Error binding");
        return 1;
    }

    // Установка на прослушивание
    int backlog = 10; // число возможных соединений в очереди
    int l = listen(listenSocket, backlog);
    if (l < 0){
        perror("Error listening");
        return 1;
    }

    // Принятие соединения
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    std::cout << "Listen socket: " << listenSocket << std::endl;
    acceptSocket = accept(listenSocket, (struct sockaddr*)&client_addr, &client_addr_size);
    if(acceptSocket < 0){
        perror("Error accept");
        return 1;
    }
}

int clientFunc(int s, struct addrinfo * servinfo){

    std::mutex _lock;
    // Принимаем приветствие
    char greetings[100];
    if (recv(s, greetings, 100, 0) < 0){
        perror("Greetings error");
        return 1;
    } else {
        std::cout << greetings << std::endl;
    }

    // Пишем сообщение
    char message[300];
    char stopword[] = "stop";
    while(1){
        _lock.lock();
        std::cout << "[User]:";
        std::cin.getline(message, 300);
        //std::cout << message << std::endl;
        if (send(s, message, 300, 0) < 0){
            perror("Caller send error");
            return 1;
        }
        if (strcmp(message, stopword)==0)
            break;
        _lock.unlock();
    }

    // Принимаем прощание
    char bye[100];
    if (recv(s, bye, 100, 0) < 0){
        perror("Bye error");
        return 1;
    } else {
        std::cout << bye << std::endl;
    }
    
    return 0;
}

int serverFunc(int s1, struct addrinfo * servinfo){

    std::mutex _lock;
    // Поприветствуем клиента:)
    if(send(s1, "Hello, motherfucker!", 30, 0) < 0){
        perror("Send error");
        return 1;
    }

    // Примем сообщение
    char message[300];
    message[1] = 'A';
    char stopword[] = "stop";
    while (strcmp(message, stopword)!=0){
        _lock.lock();
        if(recv(s1, message, 300, 0) < 0){
            perror("Recive error");
            return 1;
        } else {
            std::cout << "[Companion]:" << message << std::endl;
        }
        _lock.unlock();
    }
    // Попрощаемся с клиентом:)
    if(send(s1, "Suck my wetty balls!", 30, 0) < 0){
        perror("Listening send error");
        return 1;
    }
    return 0;
}


int main(int argc, char *argv[]){

    // Создание структур для инициализации сокета
    int status;
    struct addrinfo hints; // структура с минимальной информацией о сервере
    struct addrinfo *servinfo; // полная информация о сервере
    memset(&hints, 0, sizeof hints); // предварительное обнуление hints

    hints.ai_family = AF_INET; // любая версия IP-адреса
    hints.ai_socktype = SOCK_STREAM; // TCP connection
    
    if (argv[1]==NULL){
        hints.ai_flags = AI_PASSIVE; // автозаполнение IP-адреса сервера
        status = getaddrinfo(NULL,"3490",&hints,&servinfo); // NULL, т.к. AI_PASSIVE автоматически ставит адрес хоста
    } else {
        status = getaddrinfo(argv[1],"3490",&hints,&servinfo); // NULL, т.к. AI_PASSIVE автоматически ставит адрес хоста
        std::cout << argv[1] << std::endl;
    }        
    if (status != 0){ // обработка ошибки
        fprintf(stderr,"getaddinfo error: %sn", gai_strerror(status));
        return 1;
    }

    // Создание сокетов
    int connectSocket, listenSocket, acceptSocket; // файловые дескрипторы сокетов
    connectSocket = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if (connectSocket < 0){
        perror("Error calling socket");
        return 1;
    }

    bool isCaller;
    if(connect(connectSocket, servinfo->ai_addr, servinfo->ai_addrlen) < 0){
        close(connectSocket);
        isCaller = false;
        if (createListenAndAcceptSock(listenSocket,acceptSocket,servinfo) < 0)
            return 1;
        std::cout << "Accept socket: " << acceptSocket << std::endl;
    } else {
        std::cout << "Connect socket: " << connectSocket << std::endl;
        isCaller = true;
        if (listenSocket < 0){
            perror("Error calling socket");
            return 1;
        }
    }


    if (!isCaller) std::swap(acceptSocket, connectSocket);

    std::cout << "Is caller? - " << isCaller << std::endl;
    std::cout << "Enabled socket: " << connectSocket << std::endl;

    std::thread st(serverFunc, std::ref(connectSocket), std::ref(servinfo));
    std::thread ct(clientFunc, std::ref(connectSocket), std::ref(servinfo));

    st.join();
    ct.join();

    freeaddrinfo(servinfo); // освобождаем нахер

    close(connectSocket);
    close(listenSocket);

    return 0;
}