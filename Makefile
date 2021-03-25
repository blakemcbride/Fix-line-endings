
fix-line-endings : fix-line-endings.c
	$(CC) -O2 -o $@ $^

clean:
	rm -f *~ fix-line-endings
	
