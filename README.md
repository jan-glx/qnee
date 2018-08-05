# qnee
Quick pre-QC knee plots for barcode based scRNAseq data

## Usage
### Linux/ WSL
Compile `qnee` using e.g. `g++ -std=c++11 main.cpp -o qnee -DNDEBUG` or download a suitable binary from <TODO>.
Then run `qnee` supplying a `fastq` file for read one (cell barcode + UMI) on std_in and receiving the number of reads for each cell barcode as a text file on std_out using e.g. `zcat input_R1.fastq.gz | ./qnee > output.csv`.
Finally load and plot the aggregated counts in you favourite analysis tool. E.g., in R you can do:
```
library(ggplot2)
read_numbers <- sort(read.csv("output.csv", header = F)$V1, decreasing=TRUE)
indices <-  unique(round(exp(seq(log(10),log(1E5),length.out=101)))) # to limit the number of plotted lines
df <- data.frame(index=indices, 
                 number_of_reads=read_numbers[indices])

ggplot(df, aes(x=index,y=number_of_reads))+
  geom_line() + 
  scale_x_log10() +
  scale_y_log10()  +
  xlab("index of cell barcode sorted by total #reads") +
  ylab("#reads of cell barcode") +
  NULL
```
To get a results like this:
![Alt text](example/qnee_plot_simple.png?raw=true "Simple knee plot produced with qnee and ggplot2")
