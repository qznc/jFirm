/* Warning: Automatically generated file */
package firm.nodes;

import com.sun.jna.Pointer;

public class End extends Node {

	public End(Pointer ptr) {
		super(ptr);
	}

	

	

	

	
	public static final int pnMax = 0;

	
	public void accept(NodeVisitor visitor) {
		visitor.visit(this);
	} 
	
}