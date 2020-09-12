#!/usr/bin/env bash
FLASK_APP=flaskr flask run --host 0.0.0.0 --cert /tmp/fullchain.pem --key /tmp/privkey.pem
