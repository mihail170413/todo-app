#!/bin/sh
envsubst < /app/config.json.template > /app/config.json
exec ./todo_app
