
# Gcx


Gcx is a tool to work with GCX files found in the game Metal Gear Solid. Gcx files in Metal Gear Solid games are binary script files. Currently this tool will decompile GCX binaries into their original Game Command Language script. Currently Metal Gear Games from Metal Gear Solid 3 onwards are supported. MGS1 and MGS2 are not supported in the tool as their are some minor differences I haven't accounted for yet. Currenlty only decompilation is supported although I am hoping to add a compile function eventually. By default the -MGS4 option is selected. by entering -MGS3 at the command line you can switch it to MGS3 support. 

### GCL Information
The Game Command Language was created in-house at Kojima Productions and is based on TCL. The GCX binary generally consists of GCL compiled bytecode and resources that may or may not be encrypted. when decompiling a GCX file you can also choose to export the resources using the -res option. Resources consist of procs, strings and general binary data. The GCL Command that calls it decides how the data is interpreted.

Metal Gear Solid runs many of it strings against a custom 24 bit hash algorithm. I have represented these in hex format in the decompilation wrapped around square brackets. I have also included a dictionary that can be modified, this tries to help identify the original name of the hash. The dictionary should use EUC-JP encoding as GCL supported scripting in Japanese by running it through the hash algorithm. Commands are resolved to C functions in game. I have salvaged some commands that I know from various ELF files and have made a commands.txt which can be used to resolve them and added to if new ones are found. Using the Command and Dictionary text files is completely optional.

The scripts for all games after they are compiled should be in EUC-JP if you wish to read the Japanese print commands left behind by the developers. MGS4 is an exception which uses UTF-8 Japanese characters, however as the hashed Japanese characters in the script use EUC-JP you will have to switch between the two for this game.

There are some variable values to note for in the decompilation.
 - **$strres** calls upon a resource. Resources can be exported using the -res option
 - **$arg** is an argument that has been passed to the proc when it was called
 - **$lclArg** is variable that only lives in the current proc's scope
 - **$var** is a variable that is taken from a global gcl buffer the game creates, this buffer has memory allocated for two different regions.
  -- linkvarbuf 
  -- varbuf

there is also a localvarbuf region.

### To Do
 - Fix formatting for switch statements
 - Make Game Command Language TMLanguage.

##  Usage

By default the tool will be set to decompile in MGS4 mode and not export resources. Currenlty only decompile is supported. MGS3 and MGS4 are the only game options that can be set. MGS2 and MGS1 are not currently supported. Below is a list of games that are supported by both options;
```
-MGS4
    MGS4, MGS3D, Peace Walker
```
```
-MGS3
    MGS3, Portable Ops, Acid 1 + 2
```

If you wish to decompile an MGS4 GCX file.

```
Gcx.exe "path\to\scenerio.gcx"
```
As Decompile and MGS4 are default options there are no need to set them

```
Gcx.exe -res "path\to\scenerio.gcx"
```
If you want to export resources as well you can use the -res option. This will export numbered binary resources in a separate folder. If there are a lot of resources it can take a while.

```
Gcx.exe -mgs3 -res "path\to\scenerio.gcx"
```
If you wish to export from mgs3 with resources you would use the above option.

```
Gcx.exe -decompile -mgs4 "path\to\scenerio.gcx"
```
all of the options are optional, above is what the default would resolve to.

If there are any problems or you believe some output is incorrect please raise it in the issues tab.

[MIT](LICENSE.md)
This project falls under the MIT license.



