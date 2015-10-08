OBJECTS = AP_main.c toss_image_rtos toss_char_stor

AP_main : $(OBJECTS)
	gcc -o AP_main AP_main.c
toss_image_rtos : toss_image_rtos.c
	gcc -o toss_image_rtos toss_image_rtos.c
toss_char_stor: toss_char_stor.c
	gcc -o  toss_char_stor toss_char_stor.c

clean :
	rm -f $(OBJECTS)