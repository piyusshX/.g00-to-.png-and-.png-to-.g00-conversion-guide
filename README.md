# Guide to Covert .g00 File into .png and Repacking back to .g00
While playing a visual novel, I found a decent MTL patch that translated the text but not the CGs such as menus and episode screens. I decided to translate them myself and discovered they were stored in `.g00` files. Resources for this format are very limited, and many of the tools I found either did not work or were unavailable. After some trial and error, I figured out how to extract and repack `.g00` files (the repacking part was the main problem extraction was pretty simple). I am compiling all the info and steps in this guide so that someone like me does not have to go through all that trouble again.

---

## Tools Required
- [GARbro](https://github.com/morkt/GARbro)  For extracting `.g00` files to `.png`.

- [vaconv-cpp](https://github.com/user-attachments/files/21976204/vaconv-cpp.zip)  For converting `.png` files back into `.g00`.

---

## Extraction (g00 to png)

1. Download and install **GARbro**.  
2. Copy the path of the folder where the CGs or `.g00` files are stored.  
3. Create a folder where you want to save the converted files.  
4. Open GARbro, paste the **CG folder path** into the search bar, and hit Enter.  
5. Select all the files using **Ctrl + A**.  
6. Right-click and choose **Extract**.  
7. In the dialog, check both boxes and select **Save image as PNG**.  
8. Paste the destination folder’s path into the search bar and click **Extract**.  
9. Done, the `.g00` files are now converted into `.png`.

**Note:** Before converting back into `.g00`, make sure the dimensions of your edited `.png` match the original CG. If they differ, the images may not display correctly in the game.  

If you need to bulk resize images, here is a tutorial: [Bulk Resize Tutorial](https://youtu.be/8ic2BW9Aolo?si=0FYgVFhfIo72Ucbe)

---

## Repacking (png to g00)

1. Make sure your edited `.png` file has the **same name** as the original `.g00` file.  
   - Example:  
     - Original: `example.g00`  
     - Edited: `example.png`  

2. Place all the `.png` files in the same folder.  
3. Download **vaconv-cpp.exe** from the GitHub releases section.
4. Extract the zip file, you'll `vaconv-cpp.exe` and `bulkConvert.bat` copy and paste them in the same folder as your `.png` files.
5. Double-click bulkConvert.bat.
6. Your .png files will now be converted back into .g00 format.
