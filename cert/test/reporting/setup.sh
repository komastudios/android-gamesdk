#!/bin/sh

cp .vscode/settings.template.json .vscode/settings.json
pipenv install --dev
pipenv shell
