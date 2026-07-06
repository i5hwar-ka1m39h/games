package utils

type Rect struct {
	X      float64
	Y      float64
	Width  float64
	Height float64
}

func NewRect(x, y, width, height float64) *Rect {

	return &Rect{
		X:      x,
		Y:      y,
		Width:  width,
		Height: height,
	}
}

func (rect *Rect) MaxX() float64 {
	return rect.X + rect.Width
}
func (rect *Rect) MaxY() float64 {
	return rect.Y + rect.Height
}

func (rect *Rect) Intersect(other Rect) bool {
	return rect.X <= other.MaxX() && rect.Y <= other.MaxY() && other.X <= rect.MaxX() && other.Y <= rect.MaxY()
}
