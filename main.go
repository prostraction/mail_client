package main

import (
	"bufio"
	"crypto/tls"
	"fmt"
	"os"
)

func retrieveSecret() (login string, password string) {
	f, err := os.Open("secret")
	if err != nil {
		panic(err)
	}
	s := bufio.NewScanner(f)
	s.Split(bufio.ScanWords)
	for s.Scan() {
		if login == "" {
			login = s.Text()
		} else if password == "" {
			password = s.Text()
		}
	}
	return
}

func createConnection() {

}

var conn *tls.Conn

func sendCommand(command string) string {
	_, err := conn.Write([]byte(command + "\r\n"))
	if err != nil {
		panic(err)
	}
	var bufAll []byte
	buf := make([]byte, 1024)

	var n int
	for {
		n, err = conn.Read(buf)
		bufAll = append(bufAll, buf...)
		if n < 1024 {
			break
		}
	}
	if err != nil {
		fmt.Println(n, "bytes read.")
		panic(err)
	}
	return string(bufAll[:])
}

func readMessage() string {
	var bufAll []byte
	buf := make([]byte, 1024)

	var n int
	var err error
	for {
		n, err = conn.Read(buf)
		bufAll = append(bufAll, buf...)
		if n < 1024 {
			break
		}
	}
	if err != nil {
		fmt.Println(n, "bytes read.")
		panic(err)
	}
	return string(bufAll[:])
}

func main() {
	conf := &tls.Config{
		InsecureSkipVerify: true,
	}
	var err error
	conn, err = tls.Dial("tcp", "94.100.180.90:993", conf)
	if err != nil {
		panic(err)
	}
	defer conn.Close()

	fmt.Print(readMessage())
	log, pass := retrieveSecret()
	fmt.Print(sendCommand(". login " + log + " " + pass))
	fmt.Print(sendCommand(". select inbox"))
	fmt.Print(sendCommand(". list \"/\" \"/\""))
	fmt.Print(sendCommand(". fetch 1:* (UID FLAGS RFC822.HEADER)"))

	//fmt.Println(log, pass)
}
