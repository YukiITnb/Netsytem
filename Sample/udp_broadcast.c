int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    SOCKADDR_IN saddr,baddr,caddr;
    int clen = sizeof(caddr);
    baddr.sin_family = AF_INET;
    baddr.sin_port = htons(7000);
    baddr.sin_addr.s_addr = inet_addr("255.255.255.255");

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    bind(s, (SOCKADDR*)&saddr, sizeof(saddr));
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on));
    while (1) {
        char buffer[1024] = {0};
        int r = recvfrom(s, buffer, sizeof(buffer), 0, (SOCKADDR*)&caddr, &clen);
        if (r > 0) {
            printf("%s\n", buffer);
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&baddr, sizeof(baddr));
        } else
        break;
    }
    close(s);