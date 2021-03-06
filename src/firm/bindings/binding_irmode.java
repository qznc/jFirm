package firm.bindings;
/* WARNING: Automatically generated file */
import com.sun.jna.Native;
import com.sun.jna.Pointer;
import java.nio.Buffer;


public class binding_irmode {
	static { Native.register("firm"); }
	public static enum op_pin_state {
		op_pin_state_floats(0),
		op_pin_state_pinned(1),
		op_pin_state_exc_pinned(),
		op_pin_state_mem_pinned();
		public final int val;
		private static class C { static int next_val; }

		op_pin_state(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		op_pin_state() {
			this.val = C.next_val++;
		}
		
		public static op_pin_state getEnum(int val) {
			for(op_pin_state entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum cond_jmp_predicate {
		COND_JMP_PRED_NONE(),
		COND_JMP_PRED_TRUE(),
		COND_JMP_PRED_FALSE();
		public final int val;
		private static class C { static int next_val; }

		cond_jmp_predicate(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		cond_jmp_predicate() {
			this.val = C.next_val++;
		}
		
		public static cond_jmp_predicate getEnum(int val) {
			for(cond_jmp_predicate entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum mtp_additional_property {
		mtp_no_property(0x00000000),
		mtp_property_const(0x00000001),
		mtp_property_pure(0x00000002),
		mtp_property_noreturn(0x00000004),
		mtp_property_nothrow(0x00000008),
		mtp_property_naked(0x00000010),
		mtp_property_malloc(0x00000020),
		mtp_property_returns_twice(0x00000040),
		mtp_property_intrinsic(0x00000080),
		mtp_property_runtime(0x00000100),
		mtp_property_private(0x00000200),
		mtp_property_has_loop(0x00000400),
		mtp_property_inherited((1<<31));
		public final int val;
		private static class C { static int next_val; }

		mtp_additional_property(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		mtp_additional_property() {
			this.val = C.next_val++;
		}
		
		public static mtp_additional_property getEnum(int val) {
			for(mtp_additional_property entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum symconst_kind {
		symconst_type_tag(),
		symconst_type_size(),
		symconst_type_align(),
		symconst_addr_ent(),
		symconst_ofs_ent(),
		symconst_enum_const();
		public final int val;
		private static class C { static int next_val; }

		symconst_kind(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		symconst_kind() {
			this.val = C.next_val++;
		}
		
		public static symconst_kind getEnum(int val) {
			for(symconst_kind entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum ir_where_alloc {
		stack_alloc(),
		heap_alloc();
		public final int val;
		private static class C { static int next_val; }

		ir_where_alloc(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		ir_where_alloc() {
			this.val = C.next_val++;
		}
		
		public static ir_where_alloc getEnum(int val) {
			for(ir_where_alloc entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum ir_builtin_kind {
		ir_bk_trap(),
		ir_bk_debugbreak(),
		ir_bk_return_address(),
		ir_bk_frame_address(),
		ir_bk_prefetch(),
		ir_bk_ffs(),
		ir_bk_clz(),
		ir_bk_ctz(),
		ir_bk_popcount(),
		ir_bk_parity(),
		ir_bk_bswap(),
		ir_bk_inport(),
		ir_bk_outport(),
		ir_bk_inner_trampoline();
		public final int val;
		private static class C { static int next_val; }

		ir_builtin_kind(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		ir_builtin_kind() {
			this.val = C.next_val++;
		}
		
		public static ir_builtin_kind getEnum(int val) {
			for(ir_builtin_kind entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum pn_generic {
		pn_Generic_M(0),
		pn_Generic_X_regular(1),
		pn_Generic_X_except(2),
		pn_Generic_other(3);
		public final int val;
		private static class C { static int next_val; }

		pn_generic(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		pn_generic() {
			this.val = C.next_val++;
		}
		
		public static pn_generic getEnum(int val) {
			for(pn_generic entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum ir_value_classify_sign {
		value_classified_unknown(0),
		value_classified_positive(1),
		value_classified_negative(-1);
		public final int val;
		private static class C { static int next_val; }

		ir_value_classify_sign(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		ir_value_classify_sign() {
			this.val = C.next_val++;
		}
		
		public static ir_value_classify_sign getEnum(int val) {
			for(ir_value_classify_sign entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum ir_mode_sort_helper {
		irmsh_is_num(0x10),
		irmsh_is_data(0x20),
		irmsh_is_datab(0x40),
		irmsh_is_dataM(0x80);
		public final int val;
		private static class C { static int next_val; }

		ir_mode_sort_helper(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		ir_mode_sort_helper() {
			this.val = C.next_val++;
		}
		
		public static ir_mode_sort_helper getEnum(int val) {
			for(ir_mode_sort_helper entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum ir_mode_sort {
		irms_auxiliary(0),
		irms_control_flow(1),
		irms_memory((2|ir_mode_sort_helper.irmsh_is_dataM.val)),
		irms_internal_boolean((3|ir_mode_sort_helper.irmsh_is_datab.val)),
		irms_reference((((4|ir_mode_sort_helper.irmsh_is_data.val)|ir_mode_sort_helper.irmsh_is_datab.val)|ir_mode_sort_helper.irmsh_is_dataM.val)),
		irms_int_number(((((5|ir_mode_sort_helper.irmsh_is_data.val)|ir_mode_sort_helper.irmsh_is_datab.val)|ir_mode_sort_helper.irmsh_is_dataM.val)|ir_mode_sort_helper.irmsh_is_num.val)),
		irms_float_number(((((6|ir_mode_sort_helper.irmsh_is_data.val)|ir_mode_sort_helper.irmsh_is_datab.val)|ir_mode_sort_helper.irmsh_is_dataM.val)|ir_mode_sort_helper.irmsh_is_num.val));
		public final int val;
		private static class C { static int next_val; }

		ir_mode_sort(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		ir_mode_sort() {
			this.val = C.next_val++;
		}
		
		public static ir_mode_sort getEnum(int val) {
			for(ir_mode_sort entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static enum ir_mode_arithmetic {
		irma_uninitialized(0),
		irma_none(1),
		irma_twos_complement(2),
		irma_ones_complement(),
		irma_int_BCD(),
		irma_ieee754(256),
		irma_float_BCD(),
		irma_max();
		public final int val;
		private static class C { static int next_val; }

		ir_mode_arithmetic(int val) {
			this.val = val;
			C.next_val = val + 1;
		}
		ir_mode_arithmetic() {
			this.val = C.next_val++;
		}
		
		public static ir_mode_arithmetic getEnum(int val) {
			for(ir_mode_arithmetic entry : values()) {
				if (val == entry.val)
					return entry;
			}
			return null;
		}
	}
	public static native String get_mode_arithmetic_name(/* ir_mode_arithmetic */int ari);
	public static native Pointer new_ir_mode(String name, /* ir_mode_sort */int sort, int bit_size, int sign, /* ir_mode_arithmetic */int arithmetic, int modulo_shift);
	public static native Pointer new_ir_vector_mode(String name, /* ir_mode_sort */int sort, int bit_size, int num_of_elem, int sign, /* ir_mode_arithmetic */int arithmetic, int modulo_shift);
	public static native int is_mode(Pointer thing);
	public static native Pointer get_mode_ident(Pointer mode);
	public static native String get_mode_name(Pointer mode);
	public static native /* ir_mode_sort */int get_mode_sort(Pointer mode);
	public static native int get_mode_size_bits(Pointer mode);
	public static native int get_mode_size_bytes(Pointer mode);
	public static native int get_mode_sign(Pointer mode);
	public static native /* ir_mode_arithmetic */int get_mode_arithmetic(Pointer mode);
	public static native int get_mode_modulo_shift(Pointer mode);
	public static native int get_mode_n_vector_elems(Pointer mode);
	public static native Pointer get_mode_link(Pointer mode);
	public static native void set_mode_link(Pointer mode, Pointer l);
	public static native Pointer get_mode_min(Pointer mode);
	public static native Pointer get_mode_max(Pointer mode);
	public static native Pointer get_mode_null(Pointer mode);
	public static native Pointer get_mode_one(Pointer mode);
	public static native Pointer get_mode_minus_one(Pointer mode);
	public static native Pointer get_mode_all_one(Pointer mode);
	public static native Pointer get_mode_infinite(Pointer mode);
	public static native Pointer get_mode_NAN(Pointer mode);
	public static native Pointer get_modeF();
	public static native Pointer get_modeD();
	public static native Pointer get_modeE();
	public static native Pointer get_modeBs();
	public static native Pointer get_modeBu();
	public static native Pointer get_modeHs();
	public static native Pointer get_modeHu();
	public static native Pointer get_modeIs();
	public static native Pointer get_modeIu();
	public static native Pointer get_modeLs();
	public static native Pointer get_modeLu();
	public static native Pointer get_modeLLs();
	public static native Pointer get_modeLLu();
	public static native Pointer get_modeP();
	public static native Pointer get_modeb();
	public static native Pointer get_modeX();
	public static native Pointer get_modeBB();
	public static native Pointer get_modeM();
	public static native Pointer get_modeT();
	public static native Pointer get_modeANY();
	public static native Pointer get_modeBAD();
	public static native Pointer get_modeP_code();
	public static native Pointer get_modeP_data();
	public static native void set_modeP_code(Pointer p);
	public static native void set_modeP_data(Pointer p);
	public static native int mode_is_signed(Pointer mode);
	public static native int mode_is_float(Pointer mode);
	public static native int mode_is_int(Pointer mode);
	public static native int mode_is_reference(Pointer mode);
	public static native int mode_is_num(Pointer mode);
	public static native int mode_is_data(Pointer mode);
	public static native int mode_is_datab(Pointer mode);
	public static native int mode_is_dataM(Pointer mode);
	public static native int mode_is_float_vector(Pointer mode);
	public static native int mode_is_int_vector(Pointer mode);
	public static native int smaller_mode(Pointer sm, Pointer lm);
	public static native int values_in_mode(Pointer sm, Pointer lm);
	public static native Pointer find_unsigned_mode(Pointer mode);
	public static native Pointer find_signed_mode(Pointer mode);
	public static native Pointer find_double_bits_int_mode(Pointer mode);
	public static native int mode_honor_signed_zeros(Pointer mode);
	public static native int mode_overflow_on_unary_Minus(Pointer mode);
	public static native int mode_wrap_around(Pointer mode);
	public static native Pointer get_reference_mode_signed_eq(Pointer mode);
	public static native void set_reference_mode_signed_eq(Pointer ref_mode, Pointer int_mode);
	public static native Pointer get_reference_mode_unsigned_eq(Pointer mode);
	public static native void set_reference_mode_unsigned_eq(Pointer ref_mode, Pointer int_mode);
	public static native int is_reinterpret_cast(Pointer src, Pointer dst);
	public static native Pointer get_type_for_mode(Pointer mode);
}
