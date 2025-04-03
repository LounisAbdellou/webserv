<?php
header("HTTP 200 OK");

// Démarrer la session
session_start();

$html = "<!DOCTYPE html>
<html lang=\"fr\">
<head>
    <meta charset=\"UTF-8\">
    <title>Page 2</title>
    <style>
        body {
            font-weight: " . $_COOKIE["fontWeight"] . ";
            background-color: " . $_COOKIE["backgroundColor"] . ";
            color: " . $_COOKIE["color"] . ";
        }
    </style>
</head>
<body>
    <h1>Page 2</h1>
    <p>Les valeurs des cookies sont :</p>
    <ul>
        <li>Poids de la police : " . $_COOKIE["fontWeight"] . "</li>
        <li>Couleur de fond : " . $_COOKIE["backgroundColor"] . "</li>
        <li>Couleur du texte : " . $_COOKIE["color"] . "</li>
    </ul>
    <a href=\"page3.php\">Aller à la page 3</a>
</body>
</html>";

header("Content-Length: " . strlen($html));

echo $html;

?>
