/* Warning: Automatically generated file */
package firm.nodes;

import com.sun.jna.Pointer;

import firm.bindings.binding_ircons;
import firm.bindings.binding_irnode;
/* There are "unused" warnings in some classes,
	but suppressing these, emits warnings, because
	of useless suppress in others. Just ignore this! */

public class Div extends Node {

	public Div(Pointer ptr) {
		super(ptr);
	}

	
	
	public Node getMem() {
		return createWrapper(binding_irnode.get_Div_mem(ptr));
	}

	
	public void setMem(Node mem) {
		binding_irnode.set_Div_mem(this.ptr, mem.ptr);
	}
	
	
	public Node getLeft() {
		return createWrapper(binding_irnode.get_Div_left(ptr));
	}

	
	public void setLeft(Node left) {
		binding_irnode.set_Div_left(this.ptr, left.ptr);
	}
	
	
	public Node getRight() {
		return createWrapper(binding_irnode.get_Div_right(ptr));
	}

	
	public void setRight(Node right) {
		binding_irnode.set_Div_right(this.ptr, right.ptr);
	}
	

	
	public firm.Mode getResmode() {
		Pointer _res = binding_irnode.get_Div_resmode(ptr);
		return new firm.Mode(_res);
	}

	public void setResmode(firm.Mode _val) {
		binding_irnode.set_Div_resmode(this.ptr, _val.ptr);
	}
	
	public int getNoRemainder() {
		int _res = binding_irnode.get_Div_no_remainder(ptr);
		return _res;
	}

	public void setNoRemainder(int _val) {
		binding_irnode.set_Div_no_remainder(this.ptr, _val);
	}
	

	

	
	/** memory result */
	public static final int pnM = 0;
	
	/** control flow when no exception occurs */
	public static final int pnXRegular = 1;
	
	/** control flow when exception occured */
	public static final int pnXExcept = 2;
	
	/** result of computation */
	public static final int pnRes = 3;
	public static final int pnMax = 4;

	
	public void accept(NodeVisitor visitor) {
		visitor.visit(this);
	} 
	
}