package assets

import (
	"embed"
	"image"
	_ "image/png"
	"log"

	"github.com/hajimehoshi/ebiten/v2"
)

// below line is important this is the shit that is embeding the assets as zip like thingy
//
//go:embed *
var assets embed.FS

var PlayerImage = mustLoadImage("ship_0012.png")
var BGImg = mustLoadImage("background.png")
var EnemyImg = mustLoadImage("ship_0012.png")
var DragonImg = mustLoadImage("flying_dragon-red.png")
var BulletImg = mustLoadImage("tile_0012.png")
var FireballImg = mustLoadImage("fireball.png")
var ExplosionImg = mustLoadImage("explosion.png")
var StartScreenImg = mustLoadImage("start_screen.png")

func mustLoadImage(name string) *ebiten.Image {
	f, err := assets.Open(name)
	if err != nil {
		log.Fatalln("error loading image", name, err)
	}

	defer f.Close()

	actualImage, _, err := image.Decode(f)

	if err != nil {
		log.Fatalln("error decoding image", err)
	}

	return ebiten.NewImageFromImage(actualImage)
}
