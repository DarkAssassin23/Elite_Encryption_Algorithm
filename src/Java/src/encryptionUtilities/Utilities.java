package encryptionUtilities;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

/**
 * Class of utilities used for reading and writing the keys
 * checking if the key file exists, and deleting the key file
 * @author DarkAssassin23 - Will Jones
 *
 */
public class Utilities implements Serializable
{
	// Added to fix Serializable warning
	private static final long serialVersionUID = 1L;
	
	// Fixes bug where double-clicking on the .jar file would
	// set the working directory as a temproary location and 
	// not where the file is
	static final String keyFilePath = new File(Utilities.class.getProtectionDomain().getCodeSource().getLocation().getPath()).toString().replace("EEA_App.jar", "keys.ser");
	
	/**
	 * Saves the the array of keys as a serialized object into keys.ser
	 * @param keys Array of keys to save
	 * @return Returns true if the operation was successful
	 */
	public boolean writeKeys(String[] keys)
	{
		try 
		{
			ObjectOutputStream objOutputStream = new ObjectOutputStream(new FileOutputStream(keyFilePath));
			objOutputStream.writeObject(keys);
			objOutputStream.close();
		} 
		catch (FileNotFoundException e1) 
		{
			e1.printStackTrace();
			return false;
		}
		catch (IOException e1) 
		{
			e1.printStackTrace();
			return false;
		}
		return true;
	}
	
	/**
	 * Reads in a serialized array of keys from keys.ser
	 * @return Array of keys
	 */
	private String[] readKeys()
	{
		String[] keys = new String[] {};
		try 
		{
			ObjectInputStream objInputStream = new ObjectInputStream(new FileInputStream(keyFilePath));
			keys = (String[]) objInputStream.readObject();
			objInputStream.close();
		} 
		catch (IOException e1) 
		{
			e1.printStackTrace();
		} 
		catch (ClassNotFoundException e1) 
		{
			e1.printStackTrace();
		}
		
		return keys;
	}
	
	/**
	 * Checks if the keys.ser file exits
	 * @return If the keys.ser file exits
	 */
	public boolean keysExist()
	{
		File file = new File(keyFilePath);
		return file.exists();
	}
	
	/**
	 * If the keys.ser file exists, it reads them in and returns them
	 * otherwise it returns null
	 * @return Array of keys, or null if no key file exits
	 */
	public String[] getKeys()
	{	
		if(!keysExist())
		{
			String[] keys = null;
			
			return keys;	
		}
		
		return readKeys();
	}
	
	/**
	 * Deletes the keys.ser file containing the keys
	 * @return If the deletion was successful
	 */
	public boolean deleteKeys()
	{
		File file = new File(keyFilePath);
		return file.delete();
	}
}
