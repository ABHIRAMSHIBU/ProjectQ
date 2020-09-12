PREFIX = /usr/bin
SCRIPT = beacon.py
SERVICE = beacond.service
SERVICE_PREFIX = /etc/systemd/system/
install:
	@install $(SCRIPT) $(PREFIX)/beacon-ng
	cp -v $(SERVICE) $(SERVICE_PREFIX)
