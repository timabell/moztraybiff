.PHONY: all clean 

all: tray-biff.jar

clean:
	rm tray-biff.jar 2>/dev/null; true

tray-biff.jar: content locale
	rm $@ 2>/dev/null; zip $@ -r $^ -x '*/CVS/*'
