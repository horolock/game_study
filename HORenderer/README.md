# 3D Graphics Programming Practice 

SDL2
Windows

Linear algebra 
input : vector
output : vector

process_input(void)

update(void)
    transform_points()
        rotate(x, y, z)
        scale(amount)
        translate(amount)

render(void)
    draw_projected_points()

At triangle, 
    _ : adjacent
    | : opposite
    / : hypotenuse

sin(a) = opposite / hypotenuse
cos(a) = adjacent / hypotenuse
tan(a) = opposite / adjacent


TODO : Linear Transformations - Rotating Vectors ~ 