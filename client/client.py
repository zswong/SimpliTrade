import socket

host = "127.0.0.1"
port = 65432

def start_client():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port)) #connect to server
        while True:
            order = input("Enter order: ")
            if order.lower() == "exit":
                print("Closing client")
                break
            s.sendall(order.encode()) #encode into bytes and send
            data = s.recv(1024)
            print(f"Server response: {data.decode()}")

if __name__ == "__main__":
    start_client()