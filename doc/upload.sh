#!/bin/sh
cd out-www/
tar -czf marsyas-docs.tar.gz *
scp marsyas-docs.tar.gz gperciva@opihi.cs.uvic.ca:marsyas-docs/
ssh -l gperciva opihi.cs.uvic.ca "cd marsyas-docs/ ; tar -xf marsyas-docs.tar.gz ; mv out-www/* . ; rmdir out-www/"
rm marsyas-docs.tar.gz

