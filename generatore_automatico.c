#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void input_generate(int * s, int len);
void output_generate(int * i, int * o, int c[], int order, int len);

int main(){
	int i, order, c[7], scenario_length, *scenario_input, *scenario_output;
	FILE *fp;
	fp=fopen("testbenchgen.vhd", "w");
	srand(time(NULL));
	
	printf("Inserisci ordine: ");
	scanf("%d", &order);
	
	printf("Inserisci i 7 coefficienti: ");
	for(i=0; i<7; i++){
		scanf("%d", &c[i]);
	}
	
	printf("Lunghezza scenario: ");
	scanf(" %d", &scenario_length);
	
	scenario_input=(int *)malloc(sizeof(int)*scenario_length);
	scenario_output=(int *)malloc(sizeof(int)*scenario_length);
	
	input_generate(scenario_input, scenario_length);
	output_generate(scenario_input, scenario_output, c, order, scenario_length);
	
	fprintf(fp,"-- TB PFRL 2024-2025\n \nlibrary ieee;\nuse ieee.std_logic_1164.all;\nuse ieee.numeric_std.all;\nuse std.textio.all;\n \nentity tb2425 is\nend tb2425;\n \narchitecture project_tb_arch of tb2425 is\n\n    constant CLOCK_PERIOD : time := 20 ns;\n\n    -- Signals to be connected to the component\n    signal tb_clk : std_logic := '0';\n    signal tb_rst, tb_start, tb_done : std_logic;\n    signal tb_add : std_logic_vector(15 downto 0);\n \n    -- Signals for the memory\n    signal tb_o_mem_addr, exc_o_mem_addr, init_o_mem_addr : std_logic_vector(15 downto 0);\n    signal tb_o_mem_data, exc_o_mem_data, init_o_mem_data : std_logic_vector(7 downto 0);\n    signal tb_i_mem_data : std_logic_vector(7 downto 0);\n    signal tb_o_mem_we, tb_o_mem_en, exc_o_mem_we, exc_o_mem_en, init_o_mem_we, init_o_mem_en : std_logic;\n\n    -- Memory\n    type ram_type is array (65535 downto 0) of std_logic_vector(7 downto 0);\n    signal RAM : ram_type := (OTHERS => \"00000000\");\n \n    -- Scenario\n    type scenario_config_type is array (0 to 16) of integer;\n    constant SCENARIO_LENGTH : integer := ");
	fprintf(fp,"%d;\n",scenario_length);
	fprintf(fp,"    constant SCENARIO_LENGTH_STL : std_logic_vector(15 downto 0) := std_logic_vector(to_unsigned(SCENARIO_LENGTH, 16));\n    type scenario_type is array (0 to SCENARIO_LENGTH-1) of integer;\n\n    signal scenario_config : scenario_config_type := (to_integer(unsigned(SCENARIO_LENGTH_STL(15 downto 8))),   -- K1\n                                                      to_integer(unsigned(SCENARIO_LENGTH_STL(7 downto 0))),    -- K2\n                                                      ");
	fprintf(fp,"%d,\n                                                      ",order);
	if(order==0){
		for(i=0; i<7; i++){
			fprintf(fp, "%d, ", c[i]);
		}
		fprintf(fp, "0, 0, 0, 0, 0, 0, 0    -- C1-C14\n");
	}
	else{
		fprintf(fp,"0, 0, 0, 0, 0, 0, 0, ");
		for(i=0; i<7; i++){
			fprintf(fp, "%d%s ", c[i], i==6 ? "" : ",");
		}
		fprintf(fp, "    -- C1-C14\n");
	}
//	printf("COEFF:\n");
//	printf("[");
//	for(i=0; i<7; i++){
//		printf("%d%c ", c[i], i==6 ? ' ' : ',');
//	}
//	printf("]\n");

	fprintf(fp, "                                                      );\n    signal scenario_input : scenario_type := (");
	for(i=0; i<scenario_length; i++){
		fprintf(fp, "%d%c ", scenario_input[i], i==scenario_length-1 ? ' ' : ',');
	}
//	printf("INPUT:\n");
//	printf("[");
//	for(i=0; i<scenario_length; i++){
//		printf("%d%c ", scenario_input[i], i==scenario_length-1 ? ' ' : ',');
//	}
//	printf("]\n");
	fprintf(fp, ");\n    signal scenario_output : scenario_type :=(");
	for(i=0; i<scenario_length; i++){
		fprintf(fp, "%d%c ", scenario_output[i], i==scenario_length-1 ? ' ' : ',');
	}
	fprintf(fp, ");\n");
	fprintf(fp, "\n    signal memory_control : std_logic := '0';      -- A signal to decide when the memory is accessed\n                                                   -- by the testbench or by the project\n \n    constant SCENARIO_ADDRESS : integer := 1234;    -- This value may arbitrarily change\n \n    component project_reti_logiche is\n        port (\n                i_clk : in std_logic;\n                i_rst : in std_logic;\n                i_start : in std_logic;\n                i_add : in std_logic_vector(15 downto 0);\n \n                o_done : out std_logic;\n \n                o_mem_addr : out std_logic_vector(15 downto 0);\n                i_mem_data : in  std_logic_vector(7 downto 0);\n                o_mem_data : out std_logic_vector(7 downto 0);\n                o_mem_we   : out std_logic;\n                o_mem_en   : out std_logic\n        );\n    end component project_reti_logiche;\n \nbegin\n    UUT : project_reti_logiche\n    port map(\n                i_clk   => tb_clk,\n                i_rst   => tb_rst,\n                i_start => tb_start,\n                i_add   => tb_add,\n \n                o_done => tb_done,\n \n                o_mem_addr => exc_o_mem_addr,\n                i_mem_data => tb_i_mem_data,\n                o_mem_data => exc_o_mem_data,\n                o_mem_we   => exc_o_mem_we,\n                o_mem_en   => exc_o_mem_en\n    );\n \n    -- Clock generation\n    tb_clk <= not tb_clk after CLOCK_PERIOD/2;\n \n    -- Process related to the memory\n    MEM : process (tb_clk)\n    begin\n        if tb_clk'event and tb_clk = '1' then\n            if tb_o_mem_en = '1' then\n                if tb_o_mem_we = '1' then\n                    RAM(to_integer(unsigned(tb_o_mem_addr))) <= tb_o_mem_data after 1 ns;\n                    tb_i_mem_data <= tb_o_mem_data after 1 ns;\n                else\n                    tb_i_mem_data <= RAM(to_integer(unsigned(tb_o_mem_addr))) after 1 ns;\n                end if;\n            end if;\n        end if;\n    end process;\n \n    memory_signal_swapper : process(memory_control, init_o_mem_addr, init_o_mem_data,\n                                    init_o_mem_en,  init_o_mem_we,   exc_o_mem_addr,\n                                    exc_o_mem_data, exc_o_mem_en, exc_o_mem_we)\n    begin\n        -- This is necessary for the testbench to work: we swap the memory\n        -- signals from the component to the testbench when needed.\n \n        tb_o_mem_addr <= init_o_mem_addr;\n        tb_o_mem_data <= init_o_mem_data;\n        tb_o_mem_en   <= init_o_mem_en;\n        tb_o_mem_we   <= init_o_mem_we;\n \n        if memory_control = '1' then\n            tb_o_mem_addr <= exc_o_mem_addr;\n            tb_o_mem_data <= exc_o_mem_data;\n            tb_o_mem_en   <= exc_o_mem_en;\n            tb_o_mem_we   <= exc_o_mem_we;\n        end if;\n    end process;\n \n    -- This process provides the correct scenario on the signal controlled by the TB\n    create_scenario : process\n    begin\n        wait for 50 ns;\n \n        -- Signal initialization and reset of the component\n        tb_start <= '0';\n        tb_add <= (others=>'0');\n        tb_rst <= '1';\n \n        -- Wait some time for the component to reset...\n        wait for 50 ns;\n \n        tb_rst <= '0';\n        memory_control <= '0';  -- Memory controlled by the testbench\n \n        wait until falling_edge(tb_clk); -- Skew the testbench transitions with respect to the clock\n \n \n        for i in 0 to 16 loop\n            init_o_mem_addr<= std_logic_vector(to_unsigned(SCENARIO_ADDRESS+i, 16));\n            init_o_mem_data<= std_logic_vector(to_unsigned(scenario_config(i),8));\n            init_o_mem_en  <= '1';\n            init_o_mem_we  <= '1';\n            wait until rising_edge(tb_clk);   \n        end loop;\n \n        for i in 0 to SCENARIO_LENGTH-1 loop\n            init_o_mem_addr<= std_logic_vector(to_unsigned(SCENARIO_ADDRESS+17+i, 16));\n            init_o_mem_data<= std_logic_vector(to_unsigned(scenario_input(i),8));\n            init_o_mem_en  <= '1';\n            init_o_mem_we  <= '1';\n            wait until rising_edge(tb_clk);   \n        end loop;\n \n        wait until falling_edge(tb_clk);\n \n        memory_control <= '1';  -- Memory controlled by the component\n \n        tb_add <= std_logic_vector(to_unsigned(SCENARIO_ADDRESS, 16));\n \n        tb_start <= '1';\n \n        while tb_done /= '1' loop                \n            wait until rising_edge(tb_clk);\n        end loop;\n \n        wait for 5 ns;\n \n        tb_start <= '0';\n \n        wait;\n \n    end process;\n \n    -- Process without sensitivity list designed to test the actual component.\n    test_routine : process\n    begin\n \n        wait until tb_rst = '1';\n        wait for 25 ns;\n        assert tb_done = '0' report \"TEST FALLITO o_done !=0 during reset\" severity failure;\n        wait until tb_rst = '0';\n \n        wait until falling_edge(tb_clk);\n        assert tb_done = '0' report \"TEST FALLITO o_done !=0 after reset before start\" severity failure;\n \n        wait until rising_edge(tb_start);\n \n        while tb_done /= '1' loop                \n            wait until rising_edge(tb_clk);\n        end loop;\n \n        assert tb_o_mem_en = '0' or tb_o_mem_we = '0' report \"TEST FALLITO o_mem_en !=0 memory should not be written after done.\" severity failure;\n \n        for i in 0 to SCENARIO_LENGTH-1 loop\n            assert RAM(SCENARIO_ADDRESS+17+SCENARIO_LENGTH+i) = std_logic_vector(to_unsigned(scenario_output(i),8)) report \"TEST FALLITO @ OFFSET=\" & integer'image(17+SCENARIO_LENGTH+i) & \" expected= \" & integer'image(scenario_output(i)) & \" actual=\" & integer'image(to_integer(unsigned(RAM(SCENARIO_ADDRESS+17+SCENARIO_LENGTH+i)))) severity failure;\n        end loop;\n \n        wait until falling_edge(tb_start);\n        assert tb_done = '1' report \"TEST FALLITO o_done == 0 before start goes to zero\" severity failure;\n        wait until falling_edge(tb_done);\n \n        assert false report \"Simulation Ended! TEST PASSATO (EXAMPLE)\" severity failure;\n    end process;\n \nend architecture;\n");	
//	printf("OUTPUT:\n");
//	printf("[");
//	for(i=0; i<scenario_length; i++){
//		printf("%d%c ", scenario_output[i], i==scenario_length-1 ? ' ' : ',');
//	}
//	printf("]\n");
//	return 0;
	return 0;
}

void input_generate(int * s, int len){
	int i;
	for(i=0; i<len; i++){
		s[i]=(rand()%255)-128;
	}
	return;
}

void output_generate(int * input, int * output, int c[], int order, int len){
	int i, j, x, nastro[7]={0, 0, 0, input[0], input[1], input[2], input[3]};
	for(i=0; i<len; i++){
		if(order==0){
			output[i]=c[1]*nastro[1]+c[2]*nastro[2]+c[3]*nastro[3]+c[4]*nastro[4]+c[5]*nastro[5];
			if(output[i]>=0){
				output[i]=(output[i]>>4)+(output[i]>>6)+(output[i]>>8)+(output[i]>>10);
			}
			else{
				output[i]=(output[i]>>4)+(output[i]>>6)+(output[i]>>8)+(output[i]>>10)+4;
			}
		}
		else{
			output[i]=c[0]*nastro[0]+c[1]*nastro[1]+c[2]*nastro[2]+c[3]*nastro[3]+c[4]*nastro[4]+c[5]*nastro[5]+c[6]*nastro[6];
			if(output[i]>=0){
				output[i]=(output[i]>>6)+(output[i]>>10);
			}
			else{
				output[i]=(output[i]>>6)+(output[i]>>10)+2;
			}
		}
		
		if(output[i]>127) output[i]=127;
		if(output[i]<-128) output[i]=-128;
		
		if(i<len-4) x=input[i+4];
		else x=0;
		for(j=1; j<7; j++){
			nastro[j-1]=nastro[j];
		}
		nastro[6]=x;
	}
	return;
}

