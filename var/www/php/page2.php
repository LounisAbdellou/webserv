<?php
// Démarrer la session
session_start();
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Page 2</title>
    <style>
        body {
            font-weight: <?php echo $_COOKIE['fontWeight']; ?>;
            background-color: <?php echo $_COOKIE['backgroundColor']; ?>;
            color: <?php echo $_COOKIE['color']; ?>;
        }
    </style>
</head>
<body>
    <h1>Page 2</h1>
    <p>Les valeurs des cookies sont :</p>
    <ul>
        <li>Poids de la police : <?php echo $_COOKIE['fontWeight']; ?></li>
        <li>Couleur de fond : <?php echo $_COOKIE['backgroundColor']; ?></li>
        <li>Couleur du texte : <?php echo $_COOKIE['color']; ?></li>
    </ul>
    <a href="page3.php">Aller à la page 3</a>
</body>
</html>
