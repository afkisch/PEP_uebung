#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <termios.h>

//man 3 getopt
// sudo chmod 777 /dev/ttyACM0
int setup_tty(int tty_fd) {
    struct termios serial;
    memset(&serial, 0, sizeof(struct termios));

    serial.c_cflag =
        CS8 | CREAD | CLOCAL; // 8-bites keretméret, vétel engedélyezése, modem
                              // control tiltása
    serial.c_cc[VMIN] = 1; // karakterenkénti olvasás engedélyezése
    serial.c_cc[VTIME] = 5; // nem-kanonikus olvasás időlimitje tizedmásodpercben
    cfsetospeed(&serial, B115200); // adó sebességének beállítása
    cfsetispeed(&serial, B115200); // vevő sebességének beállítása

    // beállítások alkalmazása
    tcsetattr(tty_fd, TCSANOW, &serial);
}

int main(int argc, char* argv[]){

	int opt;
	int out_fd = -1;
	int tty_fd  = -1;

	while ((opt = getopt(argc, argv, "pf:d:")) != -1){
		switch(opt){
			case 'p':{ // parancssori parameterek kiiratasa
			 	for(int i=0; i<argc; i++){
                			printf("%s\n", argv[i]);
        			}
			}
			break;

			case 'f':{
				printf("Kimenet: %s\n", optarg);
				out_fd = open(optarg, O_CREAT | O_RDWR, 0666); // 6: oktalis 110 --> also bit 0 --> fajlkent kezel vagy valami ilyesmi, nem figyeltem
			}
			break;

			case 'd':{
				printf("Gecko: %s\n", optarg);
				tty_fd = open(optarg, O_RDWR);
			}
			break;

			default:
			break;
		}
	}

	// foprogram
	const char* str = "Eperzselé";
	write(out_fd, str, strlen(str));

	setup_tty(tty_fd);

	char buffer[80];
	ssize_t n = read(tty_fd, buffer, 80);
	write(out_fd, buffer, n);
	write(STDOUT_FILENO, buffer, n);

	close(out_fd);

	return 0;
}
