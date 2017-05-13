#!/bin/sh

git submodule sync
git submodule foreach git pull origin master
