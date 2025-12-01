FROM golang:1.25.3

WORKDIR /chat
COPY . .
RUN go mod tidy

EXPOSE 50051

CMD ["go", "run", "./cmd/chat/main.go"]