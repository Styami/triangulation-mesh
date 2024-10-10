# TP 1-2-3
## compiler et exécuter le programme
Il suffit d'avoir meson.<br>
ouvrir terminal et faire : 
```
meson compile -C build
build/MainDebug
```
## importation exportation mesh

La base de code nous permet d'importer un mesh provenant d'un fichier ``.off`` et d'exporter notre mesh en ``.off`` ou point ``obj`` au choix. Il aussi possible de tourner autour d'un point et de visualiser le résultat une fois après avoir sauvegarder notre mesh en ``.off``.
Pour ce qui est de la visualisation du mesh, il est nécessaire de passer par [ce viewer](https://3dviewer.net/index.html) pour visualiser les ``.off``. De plus, pour visualiser les ``.obj``, je me suis servie du viewer "shader_kit" proposé par Gkit2Light, la base de code développer par Jean-Claude Iehl. Il est tout de même possible de visualiser les ``.obj`` avec [ce même viewer](https://3dviewer.net/index.html).

## Calcul Laplacien
Après calcul du laplacien, nous obtenons ce rendu de mesh avec une source de lumière.
![image de la reine sans vérification des normales](data/screenshot_queen_without_verif.png)
Toutefois, on remarque que plusieurs normales sont inversées.
C'est pour cela que pour chaque normales au vertex, nous allons aussi calculer la normale au triangle de ce vertex et verifier si le produit cartésien de c'est deux vecteurs et positif et négatif. Cela nous permettra de prendre la normale opposée si cette dernière n'était pas dans la même direction que celle du triangle.
![image de la reine avec ses normales](data/screenshot_queen.png)
Nous obtenons ainsi cette image. On peut observer que certaines aspérités sont présentes sur le mesh.

## Courbure

La courbure est calculée en à la suite de chaque laplacien mais elle est normalement pas affiché car je n'ai malheureusement pas trouvé de moyen pour l'afficher à l'aide d'une couleur via dans mes ``.obj``. Je comptais utiliser des coordonnées de texture qui seraient généré en fonction de ma valeur de courbure et la texture utilisée serait [celle-ci](data/heat.png).