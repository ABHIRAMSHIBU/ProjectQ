PREFIX = /usr/bin
SCRIPT = gpsservice.py
SERVICE = gpsservice.service
SERVICE_PREFIX = /etc/systemd/system/
install:
	@install $(SCRIPT) $(PREFIX)/gpsservice
	cp -v $(SERVICE) $(SERVICE_PREFIX)
