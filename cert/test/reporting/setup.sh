#!/bin/sh
if [ ! -f .vscode/settings.json ]; then
    echo "Copying template settings file"
    cp .vscode/settings.template.json .vscode/settings.json
fi
echo "Installing python dependencies"
pipenv install --dev
pipenv shell
