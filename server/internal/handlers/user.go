package handlers

type UserHandler struct {
	ID       string `json:"id"`
	Login    string `json:"login"`
	Password string `json:"password"`
	Online   bool   `json:"online"`
}
