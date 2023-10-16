immature engine deprecated
==========================
Experimental project

Introduction:
-------------
DX 11 indie game engine.
Development stopped.

Features:
---------
* Command-Line Build ([cl.exe](https://msdn.microsoft.com/en-us/library/9s7c9wdw.aspx)).
* The fundamental 3D rendering technique is derived from 
the [DirectX 11 book](http://www.d3dcoder.net/d3d11.htm) by Frank Luna.
* Little document.
* Single core.

Compile Environment:
--------------------
Win 10/11  
Visual Studio 2019  
DirectX SDK (June 2010)  

Runtime Environment:
--------------------
Win 10/11  
DirectX End-User Runtimes (June 2010)  
Visual C++ Redistributable for Visual Studio 2019  
DirectX 11 Compatible GPU  
DirectX 9.0c Compatible Sound Card  

Dependent .Lib Files:
---------------------
* **DirectXTK.lib**: [DirectX Tool Kit](https://github.com/Microsoft/DirectXTK)
* **DirectXTKAudioDX.lib**: [DirectX Tool Kit](https://github.com/Microsoft/DirectXTK)
* **Effects11.lib**: [Effects for Direct3D 11](https://github.com/Microsoft/FX11), Debug version please rename.
* **lua.lib**: [Lua](https://www.lua.org/)
* **XInput1_3.lib**: Its orgin name is XInput.lib, from 
[DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* **X3DAudio.lib**: [DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* **XAPOFX.lib**: [DirectX SDK (June 2010)](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* **d3d11book.lib**: Derivative work form DX11 book by Frank Luna. 
[Currently this is unlicensed](https://github.com/d3dcoder/d3d12book/issues/20), can not pubic the source code. 

Compile:
--------
* **Microsoft (R) C/C++ Optimizing Compiler**:  
Debug:  
cl /EHsc /MDd /Z7 /W4 /fp:fast /bigobj  
Relase:  
cl /EHsc /MD /Ox /W4 /fp:fast /GL  

Samples:
--------
In sample\ folder, every subfolder include a signle sample.
* **init_d3d**: A basic window draw empty.
* **m3dtob3m**: Command line tool, convert text .m3d model file to binary .b3m file. 
Those two types both can be read by engine.
* **simple_scene**: A very simple scene draw 4 basic geometrics. 
[(Screen Capture)](https://github.com/endrollex/imm_engine/tree/master/sample/simple_scene/simple_scene.png)

Asset Tools:
------------
* **[immature Blender export](https://github.com/endrollex/imm_blender_export)**: 
Export Blender model data to .m3d file format for immature engine.
* **[m3dtob3m.exe](https://github.com/endrollex/imm_engine/tree/master/sample/m3dtob3m)**: 
Convert .m3d file to binary .b3m file. 
Notice .b3m file corresponding x64/x86 version.
* **CSV based scene data**: 
Use Excel to edit game scenes.
