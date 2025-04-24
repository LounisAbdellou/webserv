<?php
header("HTTP 200 OK");

// Démarrer la session
session_start();

// Mettre à jour la session
$_SESSION["message"] = "Les cookies ont été mis à jour!";

$html = "<!DOCTYPE html>
<html lang=\"fr\">
<head>
    <meta charset=\"UTF-8\">
    <title>Page 3</title>
    <style>
        body {
            font-weight: " . $_COOKIE["fontWeight"] . ";
            background-color: " . $_COOKIE["backgroundColor"] . ";
            color: " . $_COOKIE["color"] . ";
        }
    </style>
</head>
<body>
    <h1>Page 4</h1>
    <p>Les valeurs des cookies sont :</p>
    <ul>
        <li>Poids de la police : " . $_COOKIE["fontWeight"] . "</li>
        <li>Couleur de fond : " . $_COOKIE["backgroundColor"] . "</li>
        <li>Couleur du texte : " . $_COOKIE["color"] . "</li>
    </ul>
    <a href=\"index.php\">Retour à la page d'accueil</a>
</body>
</html>";

header("Content-Length: " . strlen($html));

echo $html;

?>
