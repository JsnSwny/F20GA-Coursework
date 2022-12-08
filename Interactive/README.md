# Interactive

## Models and Materials

Models were exported in the .gltf format, with textures separated in Blender. To work with the given starter code, I created each solid color as a 1024x1024 plain color texture and applied it to each material so that it could be recognised in OpenGL as it does not have functionality for using the base color. I tried different methods to include the base color from the blender export, but could not find a solution. Due to issues in the starter code with handling multiple meshes, I exported each model broken up by their meshes (e.g., trees were broken up in leaves and roots).

I imported each asset using the LoadGLTF function as follows:

```
leaves.LoadGLTF("assets/leaves.gltf");
```

I came across an issue where loading a texture for more than one model caused for the models become disformed as shown in Appendix_A. To solve this, I moved the texture loading section of the code into the DrawMesh function, so that each time a model was drawn the texture would be applied, this worked as intended.

Each model was drawn using the DrawModel function as follows:

```
	leaves.DrawModel(leaves.vaoAndEbos, leaves.model);
```

When exported from blender, I made sure that all models shared the same origin point so that they did not have to be repositioned in OpenGL. However, some models such as the knight required repositioning for the interactive part of this section. To accomplish this I had to create a new matrix for the object, following a similar format as the modelMatrix in the starter code.

This section of code defines the knightMatrix, resets the rotation and location, then adjusts the rotation after repositioning.

```
glm::mat4 knightMatrix = glm::mat4(1.0);
knightMatrix = glm::rotate(knightMatrix, modelRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
knightMatrix = glm::rotate(knightMatrix, modelRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
knightMatrix = glm::translate(knightMatrix, knightPosition);
knightMatrix = glm::rotate(knightMatrix, knightRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
knightMatrix = glm::rotate(knightMatrix, knightRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
knightMatrix = glm::scale(knightMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
```

## Lighting

Once all the models were added, I began by adding default lighting based off the Lighting lab on Canvas (https://canvas.hw.ac.uk/courses/20720/pages/12-light-models?module_item_id=1533703). I tweaked the ambience, diffusion and specular values to my liking for the scene.

In the render() method, I animated the light in a circular motion (to emulate night/day) by creating a Light matrix, which slightly rotates the light on each render.

I also created a LightBoxMatrix which shows the movements of the light. This can be seen in the 'lighting-interactive-video.mp4'.

## Interaction

There are 3 main interaction elements that I will outline below:

### Knight

The knight can be moved using the I, J, K and L keys, which adjust the knightMatrix's x and z positions as shown in 'interactive-video.mp4'. I added the ability to toggle wind by pressing 'M', which pushed the knight backwards (can be seen at 1:27 in interactive-video.mp4). The night can fly using the spacebar button which increases the knightMatrix's y poisition.

### Portals

Portals can be toggled using the 'P' key as shown at 2:05 in 'interactive-video.mp4'.

### Camera

The camera rotation can be adjusted using the arrow keys. And the x and z positions of the camera can be adjusted using the W, A, S and D keys.

## Video Links

In case there are issues with the .mp4 files, I have uploaded the interactive videos to YouTube, the links are below.

The first video (interactive-video.mp4) demonstrates the main interactions. The second video (lighting-interactive-video.mp4) gives a better demonstration of the lighting.

Interactive Video
https://youtu.be/VsZB7IDoXPI

Lighting Interactive Video
https://youtu.be/wQKKGMzB2Iw
