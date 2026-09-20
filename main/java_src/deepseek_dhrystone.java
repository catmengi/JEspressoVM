public class deepseek_dhrystone {
    // -------- Types --------
    static class Rec_Type {
        Rec_Type    Ptr_Comp;
        int         Discr;          // enum: 0..4
        int         Enum_Comp;
        int         Int_Comp;
        char[]      Str_Comp;       // 31 chars
        int         E_Comp_2;
        char[]      Str_2_Comp;     // 31 chars
        char        Ch_1_Comp;
        char        Ch_2_Comp;
    }

    static final int Ident_1 = 0;
    static final int Ident_2 = 1;
    static final int Ident_3 = 2;
    static final int Ident_4 = 3;
    static final int Ident_5 = 4;

    // -------- Globals --------
    static int       Int_Glob;
    static boolean   Bool_Glob;
    static char      Ch_1_Glob;
    static char      Ch_2_Glob;
    static int[]     Arr_1_Glob = new int[50];
    static int[][]   Arr_2_Glob = new int[50][50];

    // Sink to prevent dead-code elimination
    static int       Sink;

    // -------- Helpers --------
    static void copy31(char[] dst, char[] src) {
        int i;
        for (i = 0; i < 31; i++) dst[i] = src[i];
    }

    static boolean eq31(char[] a, char[] b) {
        int i;
        for (i = 0; i < 31; i++) {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    static char[] mkstr(char c, int len) {
        char[] s = new char[len];
        int i;
        for (i = 0; i < len; i++) s[i] = c;
        return s;
    }

    // -------- Proc_1 --------
    static void Proc_1(Rec_Type Ptr_Val_Par) {
        Rec_Type Next_Record = Ptr_Val_Par.Ptr_Comp;

        Ptr_Val_Par.Ptr_Comp = Next_Record.Ptr_Comp;
        Next_Record.Ptr_Comp = Ptr_Val_Par;
        Next_Record.Int_Comp = 5;

        Next_Record = Next_Record.Ptr_Comp;
        if (Next_Record.Discr == Ident_1) {
            Next_Record.Int_Comp = 6;
            Proc_6(Ptr_Val_Par.Enum_Comp, Next_Record);
            Next_Record = Ptr_Val_Par.Ptr_Comp;
            Proc_7(Next_Record.Int_Comp, 10, Next_Record);
        } else {
            Ptr_Val_Par = Ptr_Val_Par.Ptr_Comp;
        }
    }

    // -------- Proc_2 --------
    static void Proc_2(IntRef Int_Par_Ref) {
        int Int_Loc = Int_Par_Ref.v + 10;
        boolean Loop_While;
        int Enum_Loc = 0;

        if (Enum_Loc == Ident_1) Loop_While = false;
        else Loop_While = true;

        while (Loop_While) {
            if (Int_Loc > Int_Glob) {
                Int_Loc -= 1;
                Int_Par_Ref.v = Int_Loc - Int_Glob;
                Enum_Loc = Ident_1;
            }
            if (Enum_Loc == Ident_1) Loop_While = false;
            else Loop_While = false;    // terminates
        }
    }

    // -------- Proc_3 --------
    static void Proc_3(RefToRec Ptr_Ref_Par) {
        if (Ptr_Ref_Par.rec != null) {
            Ptr_Ref_Par.rec.Int_Comp = Int_Glob;
            Proc_7(Ptr_Ref_Par.rec.Int_Comp, 10, Ptr_Ref_Par.rec);
        }
    }

    // -------- Proc_4 --------
    static void Proc_4() {
        boolean Bool_Loc = Ch_1_Glob == 'A';
        Bool_Glob = Bool_Loc | Bool_Glob;
        Ch_2_Glob = 'B';
    }

    // -------- Proc_5 --------
    static void Proc_5() {
        Ch_1_Glob = 'A';
        Bool_Glob = false;
    }

    // -------- Proc_6 --------
    static void Proc_6(int Enum_Val_Par, Rec_Type Enum_Ref_Par) {
        Enum_Ref_Par.Enum_Comp = Enum_Val_Par;
        if (Enum_Val_Par == Ident_3) return;

        if (Enum_Val_Par < Ident_3) {
            if (Enum_Val_Par == Ident_1) return;
            if (Enum_Val_Par == Ident_2) return;
            if (Enum_Val_Par == Ident_4) return;
        } else {
            if (Enum_Val_Par == Ident_5) return;
        }
    }

    // -------- Proc_7 --------
    static void Proc_7(int Int_1_Par_Val, int Int_2_Par_Val, Rec_Type Int_Par_Ref) {
        int Int_Loc = Int_1_Par_Val + 2;
        Int_Par_Ref.Int_Comp = Int_2_Par_Val + Int_Loc;
    }

    // -------- Proc_8 --------
    static void Proc_8(int[] Arr_1_Par_Ref, int[][] Arr_2_Par_Ref,
                       int Int_1_Par_Val, int Int_2_Par_Val)
    {
        int Int_Index, Int_Loc;

        Int_Loc = Int_1_Par_Val + 5;
        Arr_1_Par_Ref[Int_Loc] = Int_2_Par_Val;
        Arr_1_Par_Ref[Int_Loc + 1] = Arr_1_Par_Ref[Int_Loc];
        Arr_1_Par_Ref[Int_Loc + 30] = Int_Loc;

        for (Int_Index = Int_Loc; Int_Index <= Int_Loc + 1; Int_Index++) {
            Arr_2_Par_Ref[Int_Loc][Int_Index] = Int_Loc;
            Arr_2_Par_Ref[Int_Loc + 1][Int_Index] = Int_Loc;
            Arr_2_Par_Ref[Int_Loc + 2][Int_Index] = Int_Loc;
        }

        Arr_2_Par_Ref[Int_Loc][Int_Loc - 1] += 1;
        Arr_2_Par_Ref[Int_Loc + 20][Int_Loc] = Arr_1_Par_Ref[Int_Loc];

        Int_Glob = 5;
    }

    // -------- Func_1 --------
    static int Func_1(char[] Ch_1_Par_Val, char[] Ch_2_Par_Val) {
        char Ch_1_Loc = Ch_1_Par_Val[0];
        char Ch_2_Loc = Ch_1_Loc;
        if (Ch_2_Loc != Ch_2_Par_Val[0]) return Ident_1;
        else {
            Ch_1_Glob = Ch_1_Loc;
            return Ident_2;
        }
    }

    // -------- Func_2 --------
    static boolean Func_2(char[] Ch_1_Par_Val, char[] Ch_2_Par_Val) {
        int Int_Loc;
        char Ch_Loc = 0;

        Ch_Loc = Ch_1_Par_Val[0];
        if (Ch_Loc < 'A' || Ch_Loc > 'Z') return true;
        else {
            Int_Loc = 2;
            while (Int_Loc <= 2) {
                if (Func_1(Ch_1_Par_Val, Ch_2_Par_Val) == Ident_1) {
                    Ch_Loc = 'A';
                    Int_Loc += 1;
                } else {
                    return false;
                }
            }
            if (Ch_Loc >= 'W' && Ch_Loc < 'Z') Int_Loc = 7;
            if (Ch_Loc == 'R') return true;
        }

        if (Ch_Loc != Ch_1_Glob) {
            Ch_1_Par_Val[0] = Ch_Loc;
            return true;
        }
        return false;
    }

    // -------- Func_3 --------
    static boolean Func_3(int Enum_Par_Val) {
        int Enum_Loc = Enum_Par_Val;
        if (Enum_Loc == Ident_3) return true;
        return false;
    }

    // -------- Small helper classes for by-ref ints/objects --------
    static class IntRef   { int    v; }
    static class RefToRec { Rec_Type rec; }

    // -------- Main driver --------
    static final char[] STR_A = { 'D','h','r','y','s','t','o','n','e',' ',
                                  'P','r','o','g','r','a','m',' ','M',' ',
                                  'N','O',' ','D','E','M','O',' ',' ',' ',' ' };

    static final char[] STR_B = { 'D','h','r','y','s','t','o','n','e',' ',
                                  'P','r','o','g','r','a','m','m','e','r',
                                  ' ','P',' ','N','O',' ','D','E','M','O',
                                  ' ',' ',' ' };

    public static int run() {
        Rec_Type   Ptr_Glob      = new Rec_Type();
        Rec_Type   Next_Ptr_Glob = new Rec_Type();
        IntRef     Int_1_Loc     = new IntRef();
        IntRef     Int_2_Loc     = new IntRef();
        IntRef     Int_3_Loc     = new IntRef();
        RefToRec   Ptr_Ref       = new RefToRec();

        int  Enum_Loc;
        int  Run_Index;
        int  Number_Of_Runs = 100000;   // tune to quota
        char[] Next_Record_Str;

        Ptr_Glob.Ptr_Comp     = Next_Ptr_Glob;
        Ptr_Glob.Discr        = Ident_1;
        Ptr_Glob.Enum_Comp    = Ident_3;
        Ptr_Glob.Int_Comp     = 40;
        Ptr_Glob.Str_Comp     = mkstr(' ', 31);
        Ptr_Glob.E_Comp_2     = 0;
        Ptr_Glob.Str_2_Comp   = mkstr(' ', 31);
        Ptr_Glob.Ch_1_Comp    = 'A';
        Ptr_Glob.Ch_2_Comp    = 'B';

        Next_Ptr_Glob.Ptr_Comp   = Ptr_Glob;
        Next_Ptr_Glob.Discr      = Ident_1;
        Next_Ptr_Glob.Enum_Comp  = Ident_3;
        Next_Ptr_Glob.Int_Comp   = 40;
        Next_Ptr_Glob.Str_Comp   = mkstr(' ', 31);
        Next_Ptr_Glob.E_Comp_2   = 0;
        Next_Ptr_Glob.Str_2_Comp = mkstr(' ', 31);
        Next_Ptr_Glob.Ch_1_Comp  = 'A';
        Next_Ptr_Glob.Ch_2_Comp  = 'B';

        Int_Glob  = 0;
        Bool_Glob = false;
        Ch_1_Glob = 'A';
        Ch_2_Glob = 'B';

        Ptr_Ref.rec = Ptr_Glob;

        for (Run_Index = 0; Run_Index < Number_Of_Runs; Run_Index++) {
            Proc_5();
            Proc_4();

            Int_1_Loc.v = 2;
            Int_2_Loc.v = 3;

            Next_Ptr_Glob = Ptr_Glob.Ptr_Comp;
            Ptr_Glob = Next_Ptr_Glob;

            Proc_3(Ptr_Ref);

            Proc_7(Int_1_Loc.v, Int_2_Loc.v, Ptr_Glob.Ptr_Comp);

            Int_3_Loc.v = 4;
            Proc_6(Ptr_Glob.Enum_Comp, Ptr_Glob.Ptr_Comp);

            Proc_7(Int_3_Loc.v, Int_3_Loc.v, Ptr_Glob.Ptr_Comp);
            Proc_8(Arr_1_Glob, Arr_2_Glob, Int_1_Loc.v, Int_3_Loc.v);

            Proc_1(Ptr_Glob);

            Enum_Loc = 0;
            while (Enum_Loc < 1) {
                if (Func_3(Ptr_Glob.Enum_Comp)) {
                    Int_Glob = 10;
                } else {
                    Int_Glob = 20;
                }
                Enum_Loc += 1;
            }

            Next_Record_Str = Ptr_Glob.Ptr_Comp.Str_Comp;
            if (Func_2(Ptr_Glob.Str_Comp, Next_Record_Str)) {
                Int_Glob = 15;
            } else {
                Int_Glob = 25;
            }

            Ptr_Ref.rec = Ptr_Glob.Ptr_Comp;
            Proc_3(Ptr_Ref);
            Int_3_Loc.v = 10;
            Proc_2(Int_3_Loc);

            if (Ch_1_Glob == 'A' &&
                Ch_2_Glob == 'B' &&
                Bool_Glob   &&
                Int_Glob == 25) {
                // nothing
            } else {
                Sink += 1;   // keeps compiler honest
            }
        }

        Sink += Int_Glob + Int_1_Loc.v + Int_2_Loc.v + Int_3_Loc.v;
        return Sink;
    }
}