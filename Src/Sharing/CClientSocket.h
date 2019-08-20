class CClientSocket  
{  
public:  
    CClientSocket();  
    virtual ~CClientSocket();  
    bool Connect(const char* ip, int port);  
    bool Send(char* sendbuf, int len);  
    int Recv(char* recvbuf, int len);
    char* GetIpByName(const char* name);
    bool Close();  
private:  
    int clientsockfd;    
}; 
