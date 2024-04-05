#!/bin/bash
#  uruchomienie 4 poleceń i zamknąć skrypt dopiero gdy wszystkie procesy potomne się zakończą

komendy=(
	"wget https://wordpress.org/latest.zip"
	"ping -c 20 wikipedia.com"
	"sleep 40"
	"find / -type f -size -100M > wynik.txt"
)


wykonaj() {
	#liczba jednoczensych poleceń
	procesy=$1

	# tablica na PID procesow potomnych
	declare -a pids

	for ((i=0; i<procesy; i++)) do
		ktory_proces=$((i%procesy))
		#uruchomianie komendy w tle
		eval "${komendy[$ktory_proces]}" &
		#zapis id PID procesu potomnego
		pids+=($!)
	done

	jobs

	for pid in "${pids[@]}"; do
		wait "$pid"
	done

	echo "Wszystkie procesy zostały zamknięte"
	
}

read -p "Wprowadz liczbe: " liczba
wykonaj "$liczba"
