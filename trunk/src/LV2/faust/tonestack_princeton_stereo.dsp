declare id "Princeton"; // in tonestack selector
declare name "Princeton";

tstack = component("tonestack.dsp");
process = tstack[tse=tstack.ts.princeton;], tstack[tse=tstack.ts.princeton;];
