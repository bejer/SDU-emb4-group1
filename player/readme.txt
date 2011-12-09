How to build

mkdir ${OVEROTOP}/user.collection/recipes/player

cp [GITROOT]/player/oe_recipe/player.bb ${OVEROTOP}/user.collection/recipes/player

cd ${OVEROTOP}/build

. profile

bitbake player

(Get some coffee)
