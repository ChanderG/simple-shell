shell: myshell.c
	cc myshell.c -lreadline -o shell
clean:
	rm shell 12CS30011_shell.zip
pack: 
	rm -rf 12CS30011_shell
	mkdir 12CS30011_shell
	cp myshell.c Makefile 12CS30011_shell/
	zip -r 12CS30011_shell.zip 12CS30011_shell
	rm -rf 12CS30011_shell


