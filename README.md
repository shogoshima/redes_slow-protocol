### Trabalho de Implementação do Protocolo SLOW

#### Como rodar?

Para rodar, execute os seguintes comandos no terminal:

```bash
g++ main.cpp slow_packet.cpp slow_peripheral.cpp -o main
./main
```

#### Funcionamento

1. Ao rodar, será realizado inicialmente uma 3-way-connect, para iniciar uma conexão com o central.

    Nessa etapa, será enviado o pacote `Connect`, receber o pacote `Setup`. Ao verificar a flag `accept` do pacote recebido, então será mandado o pacote `Data`, esperando o pacote `Ack` com a flag de `ack`.

    O exemplo de funcionamento é mostrado abaixo:
    ```
    INICIANDO 3-WAY CONNECT…
    -------------- PACOTE ENVIADO ---------------
    | sid: 00000000-0000-0000-0000-000000000000
    | sttl: 0, flags: 0x10
    | seqnum: 0
    | acknum: 0
    | window: 10000
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    Enviando pacote (32 bytes) via UDP...
    Bytes enviados: 32
    Bytes recebidos: 32
    -------------- PACOTE RECEBIDO --------------
    | sid: 6a022f12-84af-8d5e-b071-8b9380697d5a
    | sttl: 599, flags: 0x2
    | seqnum: 6936
    | acknum: 0
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    ACCEPT recebido!
    -------------- PACOTE ENVIADO ---------------
    | sid: 6a022f12-84af-8d5e-b071-8b9380697d5a
    | sttl: 599, flags: 0x4
    | seqnum: 6936
    | acknum: 6936
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    Enviando pacote (32 bytes) via UDP...
    Bytes enviados: 32
    Bytes recebidos: 32
    -------------- PACOTE RECEBIDO --------------
    | sid: 6a022f12-84af-8d5e-b071-8b9380697d5a
    | sttl: 599, flags: 0x4
    | seqnum: 6936
    | acknum: 6936
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    ACK recebido!
    3-WAY CONNECT BEM SUCEDIDO!
    ```

2. Ao estabelecer a conexão, o usuário poderá enviar dados para a central. Ele poderá escolher entre realizar uma 0-way connection ou não.

    O exemplo de funcionamento é mostrado a seguir:
    ```
    ESCREVA A MENSAGEM A SER ENVIADA (Aperte ENTER para enviar):
    (Caso queira terminar, digite 0)
    Olá, isso é a primeira mensagem
    REALIZAR UMA 0-WAY CONNECTION? (1 para sim, 0 para não)
    0
    ENVIANDO DADOS…
    -------------- PACOTE ENVIADO ---------------
    | sid: c604609c-0050-8eb9-9f6d-52b4321cfcd8
    | sttl: 599, flags: 0x4
    | seqnum: 5531
    | acknum: 5498
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 33 bytes
    | Olá, isso é a primeira mensagem
    ---------------------------------------------
    Enviando pacote (65 bytes) via UDP...
    Bytes enviados: 65
    Bytes recebidos: 32
    -------------- PACOTE RECEBIDO --------------
    | sid: c604609c-0050-8eb9-9f6d-52b4321cfcd8
    | sttl: 585, flags: 0x4
    | seqnum: 5531
    | acknum: 5531
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    ACK recebido!
    DADOS ENVIADOS COM SUCESSO

    ESCREVA A PRÓXIMA MENSAGEM A SER ENVIADA (APERTE ENTER PARA ENVIAR)
    (Caso queira terminar, digite 0)
    Estou gerando a segunda mensagem, mas com 0-way connection
    REALIZAR UMA 0-WAY CONNECTION? (1 para sim, 0 para não)
    1
    ENVIANDO DADOS…
    -------------- PACOTE ENVIADO ---------------
    | sid: c604609c-0050-8eb9-9f6d-52b4321cfcd8
    | sttl: 585, flags: 0xc
    | seqnum: 5589
    | acknum: 5531
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 58 bytes
    | Estou gerando a segunda mensagem, mas com 0-way connection
    ---------------------------------------------
    Enviando pacote (90 bytes) via UDP...
    Bytes enviados: 90
    Bytes recebidos: 32
    -------------- PACOTE RECEBIDO --------------
    | sid: c604609c-0050-8eb9-9f6d-52b4321cfcd8
    | sttl: 564, flags: 0x6
    | seqnum: 5589
    | acknum: 5589
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    ACK recebido!
    ACCEPT recebido!
    DADOS ENVIADOS COM SUCESSO

    ESCREVA A PRÓXIMA MENSAGEM A SER ENVIADA (APERTE ENTER PARA ENVIAR)
    (Caso queira terminar, digite 0)
    0
    ```

3. Quando o usuário digitar '0' no terminal, então será realizado uma desconexão.

    O exemplo de funcionamento é mostrado abaixo:
    ```
    ESCREVA A MENSAGEM A SER ENVIADA (Aperte ENTER para enviar):
    (Caso queira terminar, digite 0)
    0

    DESCONECTANDO…
    -------------- PACOTE ENVIADO ---------------
    | sid: 2bf2b218-ca6e-8576-8fe8-471fb833ac63
    | sttl: 599, flags: 0x1c
    | seqnum: 4671
    | acknum: 4671
    | window: 0
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    Enviando pacote (32 bytes) via UDP...
    Bytes enviados: 32
    Bytes recebidos: 32
    -------------- PACOTE RECEBIDO --------------
    | sid: 2bf2b218-ca6e-8576-8fe8-471fb833ac63
    | sttl: 597, flags: 0x4
    | seqnum: 0
    | acknum: 0
    | window: 1024
    | fid: 0, fo: 0
    | data_len: 0 bytes
    | 
    ---------------------------------------------
    ACK recebido!
    DESCONECTADO COM SUCESSO
    FINALIZADO
    ```

#### Funcionamentos extras

1. Janela Deslizante