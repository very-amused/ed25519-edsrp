package main

import (
	"crypto/ed25519"
	"encoding/base64"
	"fmt"
	"os"
	"strings"
)

const messageFile = "test/message.txt"

func getInput() (message, signature, publicKey []byte) {
	file, err := os.ReadFile(messageFile)
	if err != nil {
		panic(err)
	}
	lines := strings.Split(string(file), "\n")

	signedMessage, _ := base64.StdEncoding.DecodeString(lines[0])
	signature = signedMessage[0:64]
	message = signedMessage[64:]
	publicKey, _ = base64.StdEncoding.DecodeString(lines[1])
	return message, signature, publicKey
}

func main() {
	message, signature, publicKey := getInput()

	if ed25519.Verify(publicKey, message, signature) {
		fmt.Println("Message signature is OK")
		// Delete the message file if the signature is valid to prevent false positives during future runs
		if err := os.Remove(messageFile); err != nil {
			fmt.Println("Error removing message file: %v", err)
			os.Exit(1)
		}
	} else {
		fmt.Println("Invalid message signature")
		os.Exit(1)
	}
}
