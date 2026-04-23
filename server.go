package main



import (
	
    "bufio"

    "fmt"

    "log"

    "net"

    "os"


)



const (

    HOST = "127.0.0.1"

    PORT = "9000"

    TYPE = "tcp"

)



func main() {

    listen, err := net.Listen(TYPE, HOST+":"+PORT)

    if err != nil {

        log.Fatal(err)

        os.Exit(1)

    }


    defer listen.Close()

    for {

        conn, err := listen.Accept()

        if err != nil {

            log.Fatal(err)

            os.Exit(1)

        }

        go handleRequest(conn)

    }

}



func handleRequest(conn net.Conn) {

	fmt.Printf("Connected\n")
    buffer := make([]byte, 1024)

    _, err := conn.Read(buffer)

    if err != nil {

        log.Fatal(err)

    }

	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Enter command:\n")
	input, err := reader.ReadString('\n')
	if err != nil {
		
		log.Fatal(err)
	}

    responseStr := fmt.Sprintf(input)

    conn.Write([]byte(responseStr))
	
	fmt.Printf("Waiting for output:\n")
	
	for {  
        n, err := conn.Read(buffer)
        if n <= 0 {
            break;
        }
        if err != nil {
            
            log.Fatal(err)
        }
        fmt.Print(string(buffer[:n]))
    }

    
    conn.Close()


}
