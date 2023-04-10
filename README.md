# 네트워크 관련 공부

## 컴퓨터 네트워크
### [내용 정리](./computer_network/study/computernetworks.md)
### 과제 정리

### 소켓 프로그래밍
- [Lab 1](./computer_network/socket_programming/lab01/)
    - TCP와 UDP로 메세지를 전달하는 프로그램 구현
    - TCP와 UDP의 메세지 바운더리에 대한 차이를 느낌

- [HW 1](./computer_network/socket_programming/hw1/)
    - Go-Back-N 방법을 구현하여 reliable하게 텍스트 파일과 바이너리 파일을 전송하는 프로그램 구현
    - Args
        - IP : 접속할 서버 / 클라이언트의 IP
        - Port : 접속할 서버 / 클라이언트의 포트 번호
        - SWS : Sliding window size, 1부터 10까지의 정수
        - Debugging on : 디버그 메세지 On/Off
    - 바이너리 파일을 전송하는 기능 구헌의 어려움 : 데이터를 쓰고 읽는 과정에서 손상이 생김 -> string 관련 라이브러리 없이 fread/fwrite 함수를 버퍼에 곧바로 적용하여 해결
    - 패킷 구조 설계 과정 및 GBN 구현 원리, Berkeley socket API 사용법을 배움
    - 아쉬운 점 : checksum을 이용할 수 있게 설계했지만 실제 구현에서 활용되지는 않음

- [HW 2](./computer_network/socket_programming/hw2/)
    - IO multiplexing 기법을 이용하여 다중 클라이언트를 처리하는 채팅 프로그램 구현
    - Args
        - IP : 접속할 서버 / 클라이언트의 IP
        - Port : 접속할 서버 / 클라이언트의 포트 번호
        - Name : 채팅에 참여할 이름
    - Multi-Threads 외에 select() 함수를 이용하여 socket descriptor를 모니터링하고 IO multiplexing 서버를 구현하는 방법을 배움

### 개인 과제
- HW 1
    - packet switching 방식과 circuit switching 방식의 차이를 이해함

- HW 2
    - Wiresharks를 이용한 패킷 분석 방법을 경험함
    - HTTP, TCP 등의 프로토콜이 어떻게 구성되어 있고 어떤 의미를 가지는 값을 전달하는지 확인하고 분석함

- HW 3
    - Checksum 계산 방법과 GBN timeline diagram을 이해함

- HW 4
    - TCP의 동작과 RTT Estimation and Timeout 계산 방법을 이해함

- HW 5
    - TCP fields 중 congestion control에 활용되는 Nonce Sum(NS) 필드의 RFC 문서를 직접 읽으며 의미와 동작 원리를 이해함