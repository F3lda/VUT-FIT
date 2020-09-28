/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct element {
    uint64_t data;
    struct element *next;
} ELEMENT;

typedef struct {
    ELEMENT *first;
} LIST;

LIST* ListCreate();
int ListAddElement(LIST *List, uint64_t data);
ELEMENT* ListFindElement(LIST *List, uint64_t data);
int ListPopElement(LIST *List, uint64_t *data, uint64_t searched);
int ListSize(LIST *List);
int ListIsEmpty(LIST *List);
int ListClear(LIST *List);
int ListDestroy(LIST *List);
int ListError(int number);




// Buffer
LIST *GPUbufferList = NULL;


// VertexPuller
VertexPullerID GPUvertexpullerActive = emptyID;
LIST *GPUvertexpullerList = NULL;
typedef struct GPUVPindexing {
    BufferID bufferID;
	IndexType type;
} GPUVertexPullerIndexing;
typedef struct GPUVPhead {
    BufferID bufferID;
	uint64_t offset;
	uint64_t stride;
	AttributeType type;
	bool enabled;
} GPUVertexPullerHead;
typedef struct {
	GPUVPindexing Indexing;
    GPUVPhead Heads[maxAttributes];
} GPUVertexPullerSettings;


// Program (Shader)
ProgramID GPUprogramActive = emptyID;
LIST *GPUprogramList = NULL;
typedef struct {
	VertexShader vertexShader;
    FragmentShader fragmentShader;
	Uniforms uniforms;
	AttributeType type;
} GPUProgramSettings;


// FrameBuffer
uint32_t GPUframebufferWidth = 0;
uint32_t GPUframebufferHeight = 0;
uint8_t *GPUframebufferColor = nullptr;
float *GPUframebufferDepth = nullptr;



/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU(){
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
  
 	if(sizeof(void*) > sizeof(uint64_t)){
		//printf("ERROR: pointer size!\n");
	}
	
	GPUbufferList = ListCreate();
	if(GPUbufferList == NULL){
		//printf("ERROR: cannot create GPUbufferList!\n");
	}
	
	GPUvertexpullerList = ListCreate();
	if(GPUvertexpullerList == NULL){
		//printf("ERROR: cannot create GPUvertexpullerList!\n");
	}
	
	GPUprogramList = ListCreate();
	if(GPUprogramList == NULL){
		//printf("ERROR: cannot create GPUprogramList!\n");
	}
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU(){
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
  
	ListClear(GPUbufferList);
	ListDestroy(GPUbufferList);

	ListClear(GPUvertexpullerList);
	ListDestroy(GPUvertexpullerList);

	ListClear(GPUprogramList);
	ListDestroy(GPUprogramList);

	deleteFramebuffer();
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 *
 * @param size size in bytes of new buffer on GPU.
 *
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size) { 
  /// \todo Tato funkce by měla na grafické kartě vytvořit buffer dat.<br>
  /// Velikost bufferu je v parameteru size (v bajtech).<br>
  /// Funkce by měla vrátit unikátní identifikátor identifikátor bufferu.<br>
  /// Na grafické kartě by mělo být možné alkovat libovolné množství bufferů o libovolné velikosti.<br>
  	
	BufferID newBuffer = emptyID;
	if(size > 0){
		newBuffer = (BufferID)malloc(size*sizeof(char));
		if(newBuffer == (BufferID)NULL){
			newBuffer = emptyID;
		} else {
			ListAddElement(GPUbufferList, (uint64_t)newBuffer);
		}
	}
	return newBuffer;
}

/**
 * @brief This function frees allocated buffer on GPU.
 *
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer) {
  /// \todo Tato funkce uvolní buffer na grafické kartě.
  /// Buffer pro smazání je vybrán identifikátorem v parameteru "buffer".
  /// Po uvolnění bufferu je identifikátor volný a může být znovu použit při vytvoření nového bufferu.
 
	if(isBuffer(buffer)){
		free((void*)buffer);
		uint64_t temp;
		ListPopElement(GPUbufferList, &temp, (uint64_t)buffer);
	}
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const* data) {
  /// \todo Tato funkce nakopíruje data z cpu na "gpu".<br>
  /// Data by měla být nakopírována do bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje, kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) kam se data nakopírují.<br>
  /// Parametr data obsahuje ukazatel na data na cpu pro kopírování.<br>

	if(isBuffer(buffer)){
		memcpy((void*)((buffer)+(offset)), data, size);
		/*int i;
		for(i = offset; i < offset+size; i++){
			((char*)buffer)[i] = ((char*)data)[i-offset];
		}*/
	}
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void*    data)
{
  /// \todo Tato funkce vykopíruje data z "gpu" na cpu.
  /// Data by měla být vykopírována z bufferu vybraného parametrem "buffer".<br>
  /// Parametr size určuje kolik dat (v bajtech) se překopíruje.<br>
  /// Parametr offset určuje místo v bufferu (posun v bajtech) odkud se začne kopírovat.<br>
  /// Parametr data obsahuje ukazatel, kam se data nakopírují.<br>
  
 	if(isBuffer(buffer)){
		memcpy(data, (void*)((buffer)+(offset)), size);
		/*int i;
		for(i = 0; i < size; i++){
			((char*)data)[i] = ((char*)buffer)[i+offset];
		}*/
	}
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer) { 
  /// \todo Tato funkce by měla vrátit true pokud buffer je identifikátor existující bufferu.<br>
  /// Tato funkce by měla vrátit false, pokud buffer není identifikátor existujícího bufferu. (nebo bufferu, který byl smazán).<br>
  /// Pro emptyId vrací false.<br>
 
	return (buffer != emptyID && ListFindElement(GPUbufferList, (uint64_t)buffer) != NULL); 
}

/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller(){
  /// \todo Tato funkce vytvoří novou práznou tabulku s nastavením pro vertex puller.<br>
  /// Funkce by měla vrátit identifikátor nové tabulky.
  /// Prázdná tabulka s nastavením neobsahuje indexování a všechny čtecí hlavy jsou vypnuté.
  
 	VertexPullerID newVertexPuller = emptyID;
	newVertexPuller = (VertexPullerID)malloc(sizeof(GPUVertexPullerSettings));
	if(newVertexPuller == (VertexPullerID)NULL){
		newVertexPuller = emptyID;
	} else {
		ListAddElement(GPUvertexpullerList, (uint64_t)newVertexPuller);
		((GPUVertexPullerSettings*)newVertexPuller)->Indexing.bufferID = emptyID;
		((GPUVertexPullerSettings*)newVertexPuller)->Indexing.type = (IndexType)0;
		int i;
		for(i = 0; i < maxAttributes; i++){
			((GPUVertexPullerSettings*)newVertexPuller)->Heads[i].bufferID = emptyID;
			((GPUVertexPullerSettings*)newVertexPuller)->Heads[i].offset = 0;
			((GPUVertexPullerSettings*)newVertexPuller)->Heads[i].stride = 0;
			((GPUVertexPullerSettings*)newVertexPuller)->Heads[i].type = (AttributeType)0;
			((GPUVertexPullerSettings*)newVertexPuller)->Heads[i].enabled = false;
		}
	}

	return newVertexPuller;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void GPU::deleteVertexPuller(VertexPullerID vao){
  /// \todo Tato funkce by měla odstranit tabulku s nastavení pro vertex puller.<br>
  /// Parameter "vao" obsahuje identifikátor tabulky s nastavením.<br>
  /// Po uvolnění nastavení je identifiktátor volný a může být znovu použit.<br>
  
	if(isVertexPuller(vao)){
		free((void*)vao);		
		uint64_t temp;
		ListPopElement(GPUvertexpullerList, &temp, (uint64_t)vao);
	}
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void GPU::setVertexPullerHead(VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer){
  /// \todo Tato funkce nastaví jednu čtecí hlavu vertex pulleru.<br>
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "head" vybírá čtecí hlavu vybraného vertex pulleru.<br>
  /// Parametr "type" nastaví typ atributu, který čtecí hlava čte. Tímto se vybere kolik dat v bajtech se přečte.<br>
  /// Parametr "stride" nastaví krok čtecí hlavy.<br>
  /// Parametr "offset" nastaví počáteční pozici čtecí hlavy.<br>
  /// Parametr "buffer" vybere buffer, ze kterého bude čtecí hlava číst.<br>
  
	if(isVertexPuller(vao)){
		((GPUVertexPullerSettings*)vao)->Heads[head].bufferID = buffer;
		((GPUVertexPullerSettings*)vao)->Heads[head].offset = offset;
		((GPUVertexPullerSettings*)vao)->Heads[head].stride = stride;
		((GPUVertexPullerSettings*)vao)->Heads[head].type = type;
	}
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void GPU::setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer){
  /// \todo Tato funkce nastaví indexování vertex pulleru.
  /// Parametr "vao" vybírá tabulku s nastavením.<br>
  /// Parametr "type" volí typ indexu, který je uložený v bufferu.<br>
  /// Parametr "buffer" volí buffer, ve kterém jsou uloženy indexy.<br>
  
	if(isVertexPuller(vao)){
		((GPUVertexPullerSettings*)vao)->Indexing.bufferID = buffer;
		((GPUVertexPullerSettings*)vao)->Indexing.type = type;
	}
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void GPU::enableVertexPullerHead (VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce povolí čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava povolena, hodnoty z bufferu se budou kopírovat do atributu vrcholů vertex shaderu.<br>
  /// Parametr "vao" volí tabulku s nastavením vertex pulleru (vybírá vertex puller).<br>
  /// Parametr "head" volí čtecí hlavu.<br>
  
	if(isVertexPuller(vao)){
		((GPUVertexPullerSettings*)vao)->Heads[head].enabled = true;
	}
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void GPU::disableVertexPullerHead(VertexPullerID vao,uint32_t head){
  /// \todo Tato funkce zakáže čtecí hlavu daného vertex pulleru.<br>
  /// Pokud je čtecí hlava zakázána, hodnoty z bufferu se nebudou kopírovat do atributu vrcholu.<br>
  /// Parametry "vao" a "head" vybírají vertex puller a čtecí hlavu.<br>
  
	if(isVertexPuller(vao)){
		((GPUVertexPullerSettings*)vao)->Heads[head].enabled = false;
	}
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void GPU::bindVertexPuller(VertexPullerID vao){
  /// \todo Tato funkce aktivuje nastavení vertex pulleru.<br>
  /// Pokud je daný vertex puller aktivován, atributy z bufferů jsou vybírány na základě jeho nastavení.<br>
  
	GPUvertexpullerActive = vao;
}

/**
 * @brief This function deactivates vertex puller.
 */
void GPU::unbindVertexPuller(){
  /// \todo Tato funkce deaktivuje vertex puller.
  /// To většinou znamená, že se vybere neexistující "emptyID" vertex puller.
  
	GPUvertexpullerActive = emptyID;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool GPU::isVertexPuller(VertexPullerID vao){
  /// \todo Tato funkce otestuje, zda daný vertex puller existuje.
  /// Pokud ano, funkce vrací true.
  
	return (vao != emptyID && ListFindElement(GPUvertexpullerList, (uint64_t)vao) != NULL);
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID GPU::createProgram(){
  /// \todo Tato funkce by měla vytvořit nový shader program.<br>
  /// Funkce vrací unikátní identifikátor nového proramu.<br>
  /// Program je seznam nastavení, které obsahuje: ukazatel na vertex a fragment shader.<br>
  /// Dále obsahuje uniformní proměnné a typ výstupních vertex attributů z vertex shaderu, které jsou použity pro interpolaci do fragment atributů.<br>
  
 	ProgramID newProgram = emptyID;
	newProgram = (ProgramID)malloc(sizeof(GPUProgramSettings));
	if(newProgram == (ProgramID)NULL){
		newProgram = emptyID;
	} else {
		ListAddElement(GPUprogramList, (uint64_t)newProgram);
		((GPUProgramSettings*)newProgram)->vertexShader = NULL;
		((GPUProgramSettings*)newProgram)->fragmentShader = NULL;
		((GPUProgramSettings*)newProgram)->type = (AttributeType)0;
	}

	return newProgram;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void GPU::deleteProgram(ProgramID prg){
  /// \todo Tato funkce by měla smazat vybraný shader program.<br>
  /// Funkce smaže nastavení shader programu.<br>
  /// Identifikátor programu se stane volným a může být znovu využit.<br>
  
	if(isProgram(prg)){
		free((void*)prg);		
		uint64_t temp;
		ListPopElement(GPUprogramList, &temp, (uint64_t)prg);
	}
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void GPU::attachShaders(ProgramID prg, VertexShader vs, FragmentShader fs){
  /// \todo Tato funkce by měla připojít k vybranému shader programu vertex a fragment shader.
  
	((GPUProgramSettings*)prg)->vertexShader = vs;
	((GPUProgramSettings*)prg)->fragmentShader = fs;
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void GPU::setVS2FSType(ProgramID prg,uint32_t attrib,AttributeType type){
  /// \todo tato funkce by měla zvolit typ vertex atributu, který je posílán z vertex shaderu do fragment shaderu.<br>
  /// V průběhu rasterizace vznikají fragment.<br>
  /// Fragment obsahují fragment atributy.<br>
  /// Tyto atributy obsahují interpolované hodnoty vertex atributů.<br>
  /// Tato funkce vybere jakého typu jsou tyto interpolované atributy.<br>
  /// Bez jakéhokoliv nastavení jsou atributy prázdne AttributeType::EMPTY<br>
  
	((GPUProgramSettings*)prg)->type = type;
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void GPU::useProgram(ProgramID prg){
  /// \todo tato funkce by měla vybrat aktivní shader program.
  
	GPUprogramActive = prg; 
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool GPU::isProgram(ProgramID prg){
  /// \todo tato funkce by měla zjistit, zda daný program existuje.<br>
  /// Funkce vráti true, pokud program existuje.<br>

	return (prg != emptyID && ListFindElement(GPUprogramList, (uint64_t)prg) != NULL);
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform1f(ProgramID prg,uint32_t uniformId,float const&d){
  /// \todo tato funkce by měla nastavit uniformní proměnnou shader programu.<br>
  /// Parametr "prg" vybírá shader program.<br>
  /// Parametr "uniformId" vybírá uniformní proměnnou. Maximální počet uniformních proměnných je uložen v programné \link maxUniforms \endlink.<br>
  /// Parametr "d" obsahuje data (1 float).<br>
  
	((GPUProgramSettings*)prg)->uniforms.uniform[uniformId].v1 = d;
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform2f(ProgramID prg,uint32_t uniformId,glm::vec2 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 2 floaty.
  
	((GPUProgramSettings*)prg)->uniforms.uniform[uniformId].v2 = d;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform3f(ProgramID prg,uint32_t uniformId,glm::vec3 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 3 floaty.
  
	((GPUProgramSettings*)prg)->uniforms.uniform[uniformId].v3 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform4f(ProgramID prg,uint32_t uniformId,glm::vec4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává 4 floaty.
  
	((GPUProgramSettings*)prg)->uniforms.uniform[uniformId].v4 = d;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniformMatrix4f(ProgramID prg,uint32_t uniformId,glm::mat4 const&d){
  /// \todo tato funkce dělá obdobnou věc jako funkce programUniform1f.<br>
  /// Místo 1 floatu nahrává matici 4x4 (16 floatů).
  
	((GPUProgramSettings*)prg)->uniforms.uniform[uniformId].m4 = d;
}

/// @}





/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla alokovat framebuffer od daném rozlišení.<br>
  /// Framebuffer se skládá z barevného a hloukového bufferu.<br>
  /// Buffery obsahují width x height pixelů.<br>
  /// Barevný pixel je složen z 4 x uint8_t hodnot - to reprezentuje RGBA barvu.<br>
  /// Hloubkový pixel obsahuje 1 x float - to reprezentuje hloubku.<br>
  /// Nultý pixel framebufferu je vlevo dole.<br>
  
	GPUframebufferWidth = width;
	GPUframebufferHeight = height;
	GPUframebufferColor = (uint8_t*)malloc(width*height*sizeof(uint8_t)*4);
	GPUframebufferDepth = (float*)malloc(width*height*sizeof(float));
	if(GPUframebufferColor == NULL || GPUframebufferDepth == NULL){
		GPUframebufferWidth = 0;
		GPUframebufferHeight = 0;
		GPUframebufferColor = nullptr;
		GPUframebufferDepth = nullptr;
	}
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer(){
  /// \todo tato funkce by měla dealokovat framebuffer.

	if(GPUframebufferColor != nullptr){
		free(GPUframebufferColor);
		GPUframebufferColor = nullptr;
	}
	if(GPUframebufferDepth != nullptr){
		free(GPUframebufferDepth);
		GPUframebufferDepth = nullptr;
	}
	GPUframebufferWidth = 0;
	GPUframebufferHeight = 0;
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void GPU::resizeFramebuffer(uint32_t width,uint32_t height){
  /// \todo Tato funkce by měla změnit velikost framebuffer.
  
 	GPUframebufferWidth = width;
	GPUframebufferHeight = height;
	GPUframebufferColor = (uint8_t*)realloc(GPUframebufferColor, width*height*sizeof(uint8_t)*4);
	GPUframebufferDepth = (float*)realloc(GPUframebufferDepth, width*height*sizeof(float));
	if(GPUframebufferColor == NULL || GPUframebufferDepth == NULL){
		GPUframebufferWidth = 0;
		GPUframebufferHeight = 0;
		GPUframebufferColor = nullptr;
		GPUframebufferDepth = nullptr;
	}
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t* GPU::getFramebufferColor(){
  /// \todo Tato funkce by měla vrátit ukazatel na začátek barevného bufferu.<br>
  return GPUframebufferColor;
  //return &GPUframebufferColor[GPUframebufferWidth*(GPUframebufferHeight-1)];
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float* GPU::getFramebufferDepth(){
  /// \todo tato funkce by mla vrátit ukazatel na začátek hloubkového bufferu.<br>
  return GPUframebufferDepth;
  //return &GPUframebufferDepth[GPUframebufferWidth*(GPUframebufferHeight-1)];
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth(){
  /// \todo Tato funkce by měla vrátit šířku framebufferu.
  return GPUframebufferWidth;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight(){
  /// \todo Tato funkce by měla vrátit výšku framebufferu.
  return GPUframebufferHeight;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void GPU::clear(float r,float g,float b,float a){
  /// \todo Tato funkce by měla vyčistit framebuffer.<br>
  /// Barevný buffer vyčistí na barvu podle parametrů r g b a (0 - nulová intenzita, 1 a větší - maximální intenzita).<br>
  /// (0,0,0) - černá barva, (1,1,1) - bílá barva.<br>
  /// Hloubkový buffer nastaví na takovou hodnotu, která umožní rasterizaci trojúhelníka, který leží v rámci pohledového tělesa.<br>
  /// Hloubka by měla být tedy větší než maximální hloubka v NDC (normalized device coordinates).<br>
  
	if(GPUframebufferDepth != nullptr){
		int i, j;
		/*for(i = 0; i < GPUframebufferHeight*GPUframebufferWidth; i++){
				GPUframebufferDepth[i] = a;
		}*/
		for(i = 0; i < GPUframebufferHeight; i++){
			for(j = 0; j < GPUframebufferWidth; j++){
				GPUframebufferDepth[i*GPUframebufferWidth+j] = 2;
			}
		}
	}
	
	if(GPUframebufferColor != nullptr){
		int i, j;
		for(i = 0; i < GPUframebufferHeight*GPUframebufferWidth*4; i+=4){
			if(r > 1) GPUframebufferColor[i] = 255; else if(r < 0) GPUframebufferColor[i] = 0; else GPUframebufferColor[i] = r*255;
			if(g > 1) GPUframebufferColor[i+1] = 255; else if(g < 0) GPUframebufferColor[i+1] = 0; else GPUframebufferColor[i+1] = g*255;
			if(b > 1) GPUframebufferColor[i+2] = 255; else if(b < 0) GPUframebufferColor[i+2] = 0; else GPUframebufferColor[i+2] = b*255;
			if(a > 1) GPUframebufferColor[i+3] = 255; else if(a < 0) GPUframebufferColor[i+3] = 0; else GPUframebufferColor[i+3] = a*255;
		}
		/*for(i = 0; i < GPUframebufferHeight; i++){
			for(j = 0; j < GPUframebufferWidth; j++){
				GPUframebufferColor[i*GPUframebufferWidth+j] = a;
			}
		}*/
	}
}

typedef struct {
	OutVertex vertexA;
	OutVertex vertexB;
	OutVertex vertexC;
} Triangle;

void interpolate(InFragment tmp, OutVertex a, OutVertex b, OutVertex c);

void GPU::drawTriangles(uint32_t  nofVertices){
  /// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
  /// Vrcholy se budou vybírat podle nastavení z aktivního vertex pulleru (pomocí bindVertexPuller).<br>
  /// Vertex shader a fragment shader se zvolí podle aktivního shader programu (pomocí useProgram).<br>
  /// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
  
	//printf("DRAW\n");
	//printf("nofVertices: %d\n",nofVertices);
	
	if(isVertexPuller(GPUvertexpullerActive) && isProgram(GPUprogramActive)){
		// Get Vertex Shader Function
		VertexShader vertexShader = ((GPUProgramSettings *)GPUprogramActive)->vertexShader;
		FragmentShader fragmentShader = ((GPUProgramSettings *)GPUprogramActive)->fragmentShader;
		Uniforms uniforms = ((GPUProgramSettings *)GPUprogramActive)->uniforms;
		
		// Get Vertex Puller Settings
		GPUVertexPullerSettings *vertexPuller = ((GPUVertexPullerSettings *)GPUvertexpullerActive);
		bool indexingEnabled = vertexPuller->Indexing.bufferID != emptyID;
		BufferID indexingBuffer = vertexPuller->Indexing.bufferID;

		Triangle triangles[(int)ceil(nofVertices/3)];

		InVertex inVertex[nofVertices];
		OutVertex outVertex[nofVertices];
		uint64_t i,ii;
		for(ii = 0; ii < nofVertices; ii++){// Number of Vertices
			// Set Vertex ID - vertexPullerCount
			inVertex[ii].gl_VertexID = ii;
			if(indexingEnabled){
				if(vertexPuller->Indexing.type == IndexType::UINT8){uint8_t index8 = 0; getBufferData(indexingBuffer, ii*sizeof(uint8_t), sizeof(uint8_t), &index8); inVertex[ii].gl_VertexID = index8;}
				if(vertexPuller->Indexing.type == IndexType::UINT16){uint16_t index16 = 0; getBufferData(indexingBuffer, ii*sizeof(uint16_t), sizeof(uint16_t), &index16); inVertex[ii].gl_VertexID = index16;}
				if(vertexPuller->Indexing.type == IndexType::UINT32){uint32_t index32 = 0; getBufferData(indexingBuffer, ii*sizeof(uint32_t), sizeof(uint32_t), &index32); inVertex[ii].gl_VertexID = index32;}
			}
		
			for(i = 0; i < maxAttributes; i++){
				GPUVPhead head = vertexPuller->Heads[i];
				// Read Data If Head Is Enabled
				if(head.enabled){
					BufferID buffer = head.bufferID;
					uint64_t offset = head.offset;
					uint64_t stride = head.stride;
					AttributeType vertexType = head.type;
					
					// Read Data
					float tmpFloat1, tmpFloat2, tmpFloat3, tmpFloat4;
					if(vertexType == AttributeType::FLOAT){
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID, sizeof(float), &tmpFloat1);
						inVertex[ii].attributes[i].v1 = tmpFloat1;
					} else if(vertexType == AttributeType::VEC2){
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID, sizeof(float), &tmpFloat1);
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID+sizeof(float), sizeof(float), &tmpFloat2);
						inVertex[ii].attributes[i].v2 = glm::vec2(tmpFloat1,tmpFloat2);
					} else if(vertexType == AttributeType::VEC3){
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID, sizeof(float), &tmpFloat1);
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID+sizeof(float), sizeof(float), &tmpFloat2);
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID+sizeof(float)*2, sizeof(float), &tmpFloat3);
						inVertex[ii].attributes[i].v3 = glm::vec3(tmpFloat1,tmpFloat2,tmpFloat3);
					} else if(vertexType == AttributeType::VEC4){
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID, sizeof(float), &tmpFloat1);
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID+sizeof(float), sizeof(float), &tmpFloat2);
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID+sizeof(float)*2, sizeof(float), &tmpFloat3);
						getBufferData(buffer, offset + stride * inVertex[ii].gl_VertexID+sizeof(float)*3, sizeof(float), &tmpFloat4);
						inVertex[ii].attributes[i].v4 = glm::vec4(tmpFloat1,tmpFloat2,tmpFloat3,tmpFloat4);
					}
				}
			}
			
			// Call Vertex Shader
			vertexShader(outVertex[ii],inVertex[ii],uniforms);
			
			
			// Primitive Assembly
			int currentTriangle = (int)floor(ii/3);
			int currentVertex = ii%3;
			if(currentVertex == 0){
				triangles[currentTriangle].vertexA = outVertex[ii];
			} else if(currentVertex == 1){
				triangles[currentTriangle].vertexB = outVertex[ii];
			} else if(currentVertex == 2){
				triangles[currentTriangle].vertexC = outVertex[ii];
				
				Triangle triangle = triangles[currentTriangle];
			
				// Perspective division
				//A
				triangle.vertexA.gl_Position.x /= triangle.vertexA.gl_Position.w;
				triangle.vertexA.gl_Position.y /= triangle.vertexA.gl_Position.w;
				triangle.vertexA.gl_Position.z /= triangle.vertexA.gl_Position.w;

				triangle.vertexA.gl_Position.x += 1.f;
				triangle.vertexA.gl_Position.y += 1.f;

				triangle.vertexA.gl_Position.x /= 2;
				triangle.vertexA.gl_Position.y /= 2;

				triangle.vertexA.gl_Position.x *= GPUframebufferWidth;
				triangle.vertexA.gl_Position.y *= GPUframebufferHeight;
				//B
				triangle.vertexB.gl_Position.x /= triangle.vertexB.gl_Position.w;
				triangle.vertexB.gl_Position.y /= triangle.vertexB.gl_Position.w;
				triangle.vertexB.gl_Position.z /= triangle.vertexB.gl_Position.w;

				triangle.vertexB.gl_Position.x += 1.f;
				triangle.vertexB.gl_Position.y += 1.f;

				triangle.vertexB.gl_Position.x /= 2;
				triangle.vertexB.gl_Position.y /= 2;

				triangle.vertexB.gl_Position.x *= GPUframebufferWidth;
				triangle.vertexB.gl_Position.y *= GPUframebufferHeight;
				//C
				triangle.vertexC.gl_Position.x /= triangle.vertexC.gl_Position.w;
				triangle.vertexC.gl_Position.y /= triangle.vertexC.gl_Position.w;
				triangle.vertexC.gl_Position.z /= triangle.vertexC.gl_Position.w;

				triangle.vertexC.gl_Position.x += 1.f;
				triangle.vertexC.gl_Position.y += 1.f;

				triangle.vertexC.gl_Position.x /= 2;
				triangle.vertexC.gl_Position.y /= 2;

				triangle.vertexC.gl_Position.x *= GPUframebufferWidth;
				triangle.vertexC.gl_Position.y *= GPUframebufferHeight;
				
				
				// Rasterization
				
			}
		}
	}
}

/// @}
 
 
 
 
 
LIST* ListCreate()
{
    LIST *List = (LIST*)malloc(sizeof(LIST));
    if(List != NULL){
        List->first = NULL;
    }
    return List;
}

int ListAddElement(LIST *List, uint64_t data)
{
    ELEMENT *added = (ELEMENT*)malloc(sizeof(ELEMENT));
    if(added == NULL){return 1;}
    added->data = data;
    added->next = List->first;
    List->first = added;
    return 0;
}

ELEMENT* ListFindElement(LIST *List, uint64_t data)
{
    ELEMENT *temp = List->first;
    while(temp != NULL && temp->data != data){
        temp = temp->next;
    }
    return temp;
}

int ListPopElement(LIST *List, uint64_t *data, uint64_t searched)
{
    if(List->first == NULL || ListFindElement(List,searched) == NULL){
        return 2;
    }

    ELEMENT *found = ListFindElement(List,searched);
    if(found == List->first){
        *data = found->data;
        List->first = found->next;
        free(found);
    }else{
        ELEMENT *temp = List->first;
        while(temp->next != found){
            temp = temp->next;
        }
        *data = found->data;
        temp->next = found->next;
        free(found);
    }
    return 0;
}

int ListSize(LIST *List)
{
    ELEMENT *temp = List->first;
    int count = 0;
	while(temp != NULL){
        temp = temp->next;
		count++;
    }
    return count;
}


int ListIsEmpty(LIST *List)
{
    if(List->first == NULL){
        return 1;
    }
    return 0;
}

int ListClear(LIST *List)
{
    ELEMENT *temp = List->first;
    ELEMENT *destroy;
    while(temp != NULL){
        destroy = temp;
        temp = temp->next;
		free((void*)destroy->data);
        free(destroy);
    }
    List->first = NULL;
    return 0;
}

int ListDestroy(LIST *List)
{
    ELEMENT *temp = List->first;
    ELEMENT *destroy;
    while(temp != NULL){
        destroy = temp;
        temp = temp->next;
        free(destroy);
    }
    free(List);
    List = NULL;
    return 0;
}

int ListError(int number)
{
    if(number == 1){
        printf("ERROR - PRVEK NEVYTVOREN: nedostatek mista v pameti!\n");
        //exit(number);
        return 1;
    }else if(number == 2){
        printf("ERROR - PRVEK NENALEZEN: seznam je prazdny nebo prvek neexistuje!\n");
        return 2;
    }
    return 0;
}
