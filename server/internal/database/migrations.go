package database

import (
	"database/sql"

	_ "github.com/go-sql-driver/mysql"
)

func CreateUserTable(DB *sql.DB) error {
	query := `
		CREATE TABLE IF NOT EXISTS users (
            id INT PRIMARY KEY AUTO_INCREMENT,
			text_id TEXT,
            login VARCHAR(50) UNIQUE NOT NULL, 
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            password TEXT,
            online BOOLEAN DEFAULT FALSE,
			contacts JSON
        );
	`
	_, err := DB.Exec(query)
	if err != nil {
		return err
	}
	return nil
}

func UpdateContactsNull(DB *sql.DB) error {
	query := "UPDATE users SET contacts = '[]' WHERE contacts IS NULL;"
	_, err := DB.Exec(query)
	if err != nil {
		return err
	}
	return nil
}

func RunMigrations(DB *sql.DB) error {
	err := CreateUserTable(DB)
	if err != nil {
		return nil
	}
	return err
}
