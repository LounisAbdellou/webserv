#!/usr/bin/env python3

import os
import cgi
import cgitb

# Activer les rapports d'erreurs CGI
cgitb.enable()

print("<h1>Variables d'environnement CGI (Python)</h1>")
print("<pre>")
for key, value in os.environ.items():
    print(f"{key}: {value}")
print("</pre>")

print("<h2>Paramètres GET</h2>")
print("<pre>")
form = cgi.FieldStorage()
for key in form.keys():
    print(f"{key}: {form.getvalue(key)}")
print("</pre>")
