<?php
// Afficher toutes les variables d'environnement CGI
echo "<h1>Variables d'environnement CGI (PHP)</h1>";
echo "<pre>";
foreach (getenv() as $key => $value) {
    echo "$key: $value\n";
}
echo "</pre>";

// Afficher les paramètres GET et POST
echo "<h2>Paramètres GET</h2>";
echo "<pre>";
print_r($_GET);
echo "</pre>";

echo "<h2>Paramètres POST</h2>";
echo "<pre>";
print_r($_POST);
echo "</pre>";
?>
