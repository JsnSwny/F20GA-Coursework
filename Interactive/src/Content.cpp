#include "Content.hpp"


Content::Content() {

}

void Content::LoadGLTF(string filename){

    cout << "Trying to load model " << filename << "\n";

	TinyGLTF loader;
	string err;
    string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	if (!warn.empty()) cout << "WARNING - GLTF: " << warn << endl;
	if (!err.empty())  cout << "ERROR - GLTF: " << err << endl;
	
	if (!res)
		cout << "ERROR - Failed to load glTF: " << filename << endl;
	else
		cout << "OK - Loaded glTF: " << filename << endl;

	vaoAndEbos = BindModel(model);

}


void Content::BindMesh(map<int, GLuint> &vbos, Model &model, Mesh &mesh)
{
	for (size_t i = 0; i < model.bufferViews.size(); ++i)
	{
		const BufferView &bufferView = model.bufferViews[i];
		if (bufferView.target == 0)
		{ // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero" << std::endl;
			continue; // Unsupported bufferView.
					  /*
						From spec2.0 readme:
						https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
								 ... drawArrays function should be used with a count equal to
						the count            property of any of the accessors referenced by the
						attributes            property            (they are all equal for a given
						primitive).
					  */
		}

		const Buffer &buffer = model.buffers[bufferView.buffer];
		cout << "bufferview.target " << bufferView.target << endl;

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[i] = vbo;
		glBindBuffer(bufferView.target, vbo);

		cout << "buffer.data.size = " << buffer.data.size()
				  << ", bufferview.byteOffset = " << bufferView.byteOffset
				  << std::endl;

		glBufferData(bufferView.target, bufferView.byteLength,
					 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		Primitive primitive = mesh.primitives[i];
		Accessor indexAccessor = model.accessors[primitive.indices];

		for (auto &attrib : primitive.attributes)
		{
			Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR)
			{
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0)
				vaa = 0;
			if (attrib.first.compare("NORMAL") == 0)
				vaa = 2;
			if (attrib.first.compare("TEXCOORD_0") == 0)
				vaa = 1;

			if (vaa > -1)
			{
				glEnableVertexAttribArray(vaa);
                glVertexAttribPointer(vaa, size, accessor.componentType,
                    accessor.normalized ? GL_TRUE : GL_FALSE,
                    byteStride, ((char*)NULL + accessor.byteOffset));
			}
			else
				std::cout << "vaa missing: " << attrib.first << std::endl;

		}
	}
}

// bind models
void Content::BindModelNodes(map<int, GLuint> &vbos, Model &model, Node &node)
{
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
	{
		BindMesh(vbos, model, model.meshes[node.mesh]);
	}

	for (size_t i = 0; i < node.children.size(); i++)
	{
		assert((node.children[i] >= 0) && (node.children[i] < model.nodes.size()));
		BindModelNodes(vbos, model, model.nodes[node.children[i]]);
	}
}

pair<GLuint, map<int, GLuint>> Content::BindModel(Model &model)
{
	map<int, GLuint> vbos;
	GLuint vao;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	const Scene &scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		assert((scene.nodes[i] >= 0) && (scene.nodes[i] < model.nodes.size()));
		BindModelNodes(vbos, model, model.nodes[scene.nodes[i]]);
	}

	glBindVertexArray(0);

	// cleanup vbos but do not delete index buffers yet
	for (auto it = vbos.cbegin(); it != vbos.cend();)
	{
		BufferView bufferView = model.bufferViews[it->first];
		if (bufferView.target != GL_ELEMENT_ARRAY_BUFFER)
		{
			glDeleteBuffers(1, &vbos[it->first]);
			vbos.erase(it++);
		}
		else
		{
			++it;
		}
	}

	return {vao, vbos};
}

void Content::DrawMesh(const map<int, GLuint> &vbos, Model &model, Mesh &mesh)
{
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{

		Texture &tex = model.textures[i];
		if (tex.source > -1)
		{

			GLuint texid;
			glGenTextures(1, &texid);

			tinygltf::Image &image = model.images[tex.source];

			glBindTexture(GL_TEXTURE_2D, texid);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			GLenum format = GL_RGBA;

			GLenum type = GL_UNSIGNED_BYTE;

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0,
							format, GL_UNSIGNED_BYTE, &image.image.at(0));
		}

		Primitive primitive = mesh.primitives[i];
		Accessor indexAccessor = model.accessors[primitive.indices];

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbos.at(indexAccessor.bufferView));

		glDrawElements(primitive.mode, indexAccessor.count,
            indexAccessor.componentType,
            ((char*)NULL + indexAccessor.byteOffset));
	}
}

// recursively draw node and children nodes of model
void Content::DrawModelNodes(const pair<GLuint, map<int, GLuint>> &vaoAndEbos, Model &model, Node &node)
{
	if ((node.mesh >= 0) && (node.mesh < model.meshes.size()))
	{
		DrawMesh(vaoAndEbos.second, model, model.meshes[node.mesh]);
	}
	for (size_t i = 0; i < node.children.size(); i++)
	{
		DrawModelNodes(vaoAndEbos, model, model.nodes[node.children[i]]);
	}
}

void Content::DrawModel(const pair<GLuint, map<int, GLuint>> &vaoAndEbos, Model &model)
{

	glBindVertexArray(vaoAndEbos.first);    

	const Scene &scene = model.scenes[model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i)
	{
		DrawModelNodes(vaoAndEbos, model, model.nodes[scene.nodes[i]]);
	}
}

Content::~Content(){
	
}