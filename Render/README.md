# Render

## Modelling Process

### The Scrapped Project

I built my models as I was learning how to use blender and before understanding the export/import process with unreal engine 5. Some of my models were problematic during the UE5 importing process and so I had to choose to scrap the project and start again. However, I'll explain the modelling process that went into the items that didn't make it to the final version, the blender file can be found in '\Blender_Project_Files_V2'.

Columns - The columns were the most detailed model in this project. I made use of bevels and extrusions to give subtle detail to the columns. One problem may have been the inward extrusion on the vertical pillar, as this added a high number of vectors and became difficult to UV map later on. I used a reference to model the gold decoration at the head of the column, sketching it in 2D and then extruding it to turn it into a 3D object. I then used the mirror modifier to align them well with the pillar. To create the column arch I used the spin tool and mirrored the columns to connect the arch perfectly. An array modifier was also used to create multiple of the same columns in a row. I applied a baked texture to this model and UV mapped it to make it look as realistic as possible.

Brick Floor/Wall - The brick floor was created using a procedural texture which made use of the brick texture node. This was also used for the wall that went around the pillars.

Landscape - I made use of the A.N.T Landscape mesh which allowed me to generate a mountainous scenery in the backround. I used a rock material and snow material passed into a mix shader to create a snowy effect on the mountain.

### The Final Project

Columns - Learning from my mistakes, I chose to create a less detailed column but still included some minor details. I scaled to top of the pillar inwards to give a large bottom > small top effect on the pillar. I used some bevel/extrusions to give some details to the head of the pillar. Again, I used the spin tool to connect an arch between the columns, but made sure to be careful of overlap and excessive vectors. I applied

Portals - The portals were simply created using a plane and I created the material using an emission node and a glass node which took a textured effect to make it look like a portal.

Rocks - The rocks were created using a UV sphere and then modelled to my liking using the proportional editting. I applied a various solid colours of gray to the different rocks.

Trees - I created the branches using straight lines and then applied the skin modifier to make it look like a branch. I then created the leaves similarly to how I modelled the rocks using UV spheres, I applied a Displace modifier to give it a more low poly effect. I applied a solid green and brown colour to make it look like a tree.

Water - I created the water from a circle and created the material using by setting the colours to blue and increasing the transmission.

Scenery - I created a plane and used proportional editting to give a mountainous low poly effect to the scenery and applied a green texture.

Building - I created steps using cubes with various solid colours. I then created the roof using a cube and added a loop cut to the middle to allow me to create a pointed roof. I added some minor details to the sides of the building by adding multiple loop cuts and extruding the faces.

Knight - I created a knight using various cubes and spheres and aplpied a silver material to them. I created a shield with a gold texture applied and a sword with various gold/silver textures applied.

The scene was set up using a dynamic sky to give the lighting effect, and the objects were positioned within the landscape that I created.

I used the Cycles engine set to 4096 samples for the final render and turned on the bloom setting to give the glowing effect for the portals.
