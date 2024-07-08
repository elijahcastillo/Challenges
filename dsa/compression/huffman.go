package main 


import (
	"fmt"
	"encoding/json"
	"strings"
	"os"
)

type HuffmanEncoding struct {
	EncodedString string
	CodeMap map[string]string
	Tree HuffmanNode
}

func NewHuffmanEncoding(input string) HuffmanEncoding {
	freqMap := createFrequencyMap(input)
	tree := buildHuffmanTree(freqMap)
	codeMap := getHuffmanCodesMap(&tree)
	encodedString := encodeToString(input, codeMap)

	return HuffmanEncoding{
		EncodedString: encodedString,
		CodeMap: codeMap,
		Tree: tree,
	}
}

func (huff HuffmanEncoding) toFile(filename string){
    file, err := os.Create(filename)
    if err != nil {
        return err
    }
    defer file.Close()

    var b byte
    bits := 0
    for _, char := range huff.EncodedString {
		var value byte = 0x01
		if string(char) == "0"{
			value = 0x00
		}

		b = b << 1 | value
		bits++
		
		if bits == 8 {
			// Encoded a full bit of data, write to file
            if _, err := file.Write([]byte{b}); err != nil {
                return err
            }
            // Reset for the next byte
            b = 0
            bits = 0
		}
	}

	if bits > 0 {
		// Encode remaining bits
        b <<= (8 - bits) // Shift the bits to the high-order bits of the byte
        if _, err := file.Write([]byte{b}); err != nil {
            return err
        }
	}

	return nil
}

func createFrequencyMap(input string) map[string]int {
	freqMap := make(map[string]int)

	// Find frequency of each character in input
	for i := 0; i < len(input); i++ {
		var char string = string(input[i])
		if _, ok := freqMap[char]; ok {
			freqMap[char]++
		} else {
			freqMap[char] = 1
		}	
	}

	return freqMap;
}

func buildHuffmanTree(freqMap map[string]int) HuffmanNode {
	heap := newHuffmanMinHeap()
	for k, v := range freqMap {
		heap.insert(newHuffmanNode(k, v))
	}

	for heap.Size != 1 {

		// Create new internal node with sum of freq
		left := heap.extractMin()
		right := heap.extractMin()
		top := newHuffmanNode("$", left.Freq + right.Freq)

		// Update internal nodes pointers
		top.Left = &left;
		top.Right = &right;

		// This is the cool part, you re-insert this internal node into the heap
		// Doing this will eventally cause the huffman tree to be created since it
		// will keep extracting the min from the heap which can be other internal nodes
		heap.insert(top);
	}

	fmt.Println(heap.Values)

	// The min of the heap is root of Huffman Tree
	return heap.extractMin()
}

func getHuffmanCodesMap(tree *HuffmanNode) map[string]string{
	codeMap := make(map[string]string)
	getCodesHelper(tree, "", codeMap);	
	return codeMap;
}

func getCodesHelper(node *HuffmanNode, code string, codeMap map[string]string){

	if node == nil {
		return
	}

	if node.Left != nil {
		// Append 0 to the code path for left child
		getCodesHelper(node.Left, code + "0", codeMap);
	}

	if node.Right != nil {
		// Append 1 to the code path for right child
		getCodesHelper(node.Right, code + "1", codeMap);
	}
	
	if node.isLeaf() {
		codeMap[node.Char] = code
		fmt.Printf("%v: %v\n", node.Char, code);
	}

}



type HuffmanNode struct {
	Freq  int
	Char  string
	Left  *HuffmanNode
	Right *HuffmanNode
}

func newHuffmanNode(char string, freq int) HuffmanNode {
	return HuffmanNode{
		Freq: freq,
		Char: char,
		Left: nil,
		Right: nil,
	}

}


func (n HuffmanNode) isLeaf() bool {
	return (n.Left == nil) && (n.Right == nil)
}


type HuffmanMinHeap struct {
	Values []HuffmanNode
	Size int
}

func newHuffmanMinHeap() *HuffmanMinHeap {
	return &HuffmanMinHeap{
		Values: []HuffmanNode{},
		Size: 0,
	}
}

func (heap HuffmanMinHeap) parent(index int) int{
	return (index - 1) / 2
}

func (heap *HuffmanMinHeap) insert(node HuffmanNode){
	heap.Values = append(heap.Values, node);
	heap.Size++;

	// Starts at newly inserted value
	var index int = len(heap.Values) - 1;

	// Heapify up the tree
	for index > 0 {
		parentIdx := heap.parent(index);
		if heap.Values[index].Freq >= heap.Values[parentIdx].Freq {
			break;
		}

		// Swap
		heap.Values[index], heap.Values[parentIdx] = heap.Values[parentIdx], heap.Values[index]
		index = parentIdx;
	}
}

func (heap *HuffmanMinHeap) extractMin() HuffmanNode {
	if heap.Size == 0 {
		// TODOO: Return an error?
		return HuffmanNode{}
	}

	minElement := heap.Values[0]
	lastElement := heap.Values[heap.Size - 1]
	heap.Values[0] = lastElement
	heap.Values = heap.Values[:heap.Size-1] // Remove the last element by slicing it off
	heap.Size--

	if heap.Size > 0 {
		heap.heapify(0)
	}

	return minElement
}



func (heap *HuffmanMinHeap) heapify(index int) {
	smallest := index
	left := 2*index + 1
	right := 2*index + 2

	if left < heap.Size && heap.Values[left].Freq < heap.Values[index].Freq {
		smallest = left
	}

	if right < heap.Size && heap.Values[right].Freq < heap.Values[index].Freq {
		smallest = right
	}

	if smallest != index {
		heap.Values[index], heap.Values[smallest] = heap.Values[smallest], heap.Values[index]
		heap.heapify(smallest)
	}
}







func encodeToString(input string, codeMap map[string]string) string {
	var output strings.Builder

	for i := 0; i < len(input); i++ {
		var char string = string(input[i])
		if encoding, ok := codeMap[char]; ok {
			output.WriteString(encoding);
		}
	}

	return output.String()
}

func encodeToFile(encodedString string, filename string) error{
    file, err := os.Create(filename)
    if err != nil {
        return err
    }
    defer file.Close()

    var b byte
    bits := 0
    for _, char := range encodedString {
		var value byte = 0x01
		if string(char) == "0"{
			value = 0x00
		}

		b = b << 1 | value
		bits++
		
		if bits == 8 {
			// Encoded a full bit of data, write to file
            if _, err := file.Write([]byte{b}); err != nil {
                return err
            }
            // Reset for the next byte
            b = 0
            bits = 0
		}
	}

	if bits > 0 {
		// Encode remaining bits
        b <<= (8 - bits) // Shift the bits to the high-order bits of the byte
        if _, err := file.Write([]byte{b}); err != nil {
            return err
        }
	}

	return nil
}


func main(){
	var input string = "lollipdfaop"
	// var input string = "The quic brown fox jumped over eht lzay dog in which we can all be come the best in the world"
	
	freqMap := createFrequencyMap(input)
	tree := buildHuffmanTree(freqMap)
	codeMap := getHuffmanCodesMap(&tree)
	encodedString := encodeToString(input, codeMap)

	// Write actual bits to file 8 at a time (account for less that 8 bits at end)
	encodeToFile(encodedString, "test.txt")

    bs, _ := json.Marshal(codeMap)
    fmt.Println(string(bs))

}
