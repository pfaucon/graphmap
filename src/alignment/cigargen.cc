/*
 * cigargen.cc
 *
 *  Created on: Aug 28, 2014
 *      Author: ivan
 */

#include "alignment/cigargen.h"
#include "utility/utility_general.h"



std::string AlignmentToCigar(unsigned char *alignment, int alignmentLength, bool extended_format) {
  if (alignment == NULL || alignmentLength == 0)
    return (std::string("*-"));

  std::stringstream ss;

  char* cigar = NULL;
  if (extended_format == true) {
    AlignmentToExtendedCigar(alignment, alignmentLength, &cigar);
  } else {
    AlignmentToBasicCigar(alignment, alignmentLength, &cigar);
  }
  ss << cigar;
  if (cigar)
    free(cigar);

  return ss.str();
}

std::string ReverseCigarString(std::string &cigar) {
  std::stringstream ret_cigar;

  int64_t num_cigarops = 0;
  for (int64_t i=0; i<((int64_t) cigar.size()); i++)
    if (cigar[i] < '0' || cigar[i] > '9')
      num_cigarops += 1;

  std::vector<std::string> cigarops(num_cigarops, "");

  std::string current_cigar = "";
  for (int64_t i=0; i<((int64_t) cigar.size()); i++) {
    current_cigar += cigar[i];
    if (cigar[i] < '0' || cigar[i] > '9') {
      cigarops.push_back(current_cigar);
      current_cigar = "";
    }
  }

  for (int64_t i=0; i<((int64_t) cigarops.size()); i++) {
    ret_cigar << cigarops[cigarops.size() - i - 1];
  }

  cigarops.clear();

  return ret_cigar.str();
}

int ConvertInsertionsToClipping(unsigned char* alignment, int alignmentLength) {
  for (int64_t i=0; i<alignmentLength; i++) {
    if (alignment[i] == EDLIB_I)
      alignment[i] = EDLIB_S;
    else break;
  }
  for (int64_t i=(((int64_t) alignmentLength) - 1); i>=0; i--) {
    if (alignment[i] == EDLIB_I)
      alignment[i] = EDLIB_S;
    else break;
  }

  return 0;
}

int CountClippedBases(unsigned char* alignment, int alignmentLength, int64_t *ret_num_clipped_front, int64_t *ret_num_clipped_back) {
  if (ret_num_clipped_front == NULL || ret_num_clipped_back == NULL)
    return 1;

  int64_t num_clipped_front = 0, num_clipped_back = 0;
  for (int64_t i=0; i<alignmentLength; i++) {
    if (alignment[i] == EDLIB_S || alignment[i] == EDLIB_I)
      num_clipped_front += 1;
    else break;
  }
  for (int64_t i=(((int64_t) alignmentLength) - 1); i>=0; i--) {
    if (alignment[i] == EDLIB_S || alignment[i] == EDLIB_I)
      num_clipped_back += 1;
    else break;
  }

  *ret_num_clipped_front = num_clipped_front;
  *ret_num_clipped_back = num_clipped_back;

  return 0;
}

std::vector<unsigned char> FixAlignment(unsigned char* alignment, int alignmentLength) {
  int move_type = -1, prev_move_type = -1;
  int64_t num_moves = 0, prev_num_moves = 0;
  std::vector<unsigned char> new_alignment;
  new_alignment.reserve(alignmentLength);

  for (int64_t i=0; i<alignmentLength; i++) {

  }

  return new_alignment;
}

std::vector<unsigned char> FixAlignment1(unsigned char* alignment, int alignmentLength) {
  int move_type = -1, prev_move_type = -1;
  int64_t num_moves = 0, prev_num_moves = 0;
  std::vector<unsigned char> new_alignment;
  new_alignment.reserve(alignmentLength);
//  return new_alignment;

  for (int64_t i=0; i<alignmentLength; i++) {
//    printf ("[%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", i, prev_num_moves, num_moves, prev_move_type, move_type);
//    fflush(stdout);

    if (i == 0) {
      // This is the first op.
      move_type = alignment[i];
      num_moves = 1;
    } else if (alignment[i] != alignment[i-1]) {
      // Operation changed. Check if there was another one before it, or was the current streak the first one in the alignment.
      if (prev_move_type != -1) {
        if ((move_type == EDLIB_D && prev_move_type == EDLIB_I) || (move_type == EDLIB_I && prev_move_type == EDLIB_D)) {
          if (prev_num_moves < num_moves) {
            std::vector<int> temp_op(prev_num_moves, EDLIB_X);
            new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//            printf ("-> (1) [%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", i, prev_num_moves, num_moves, prev_move_type, move_type);
//            fflush(stdout);
            num_moves = num_moves - prev_num_moves;

          } else if (prev_num_moves > num_moves) {
            std::vector<int> temp_op(prev_num_moves - num_moves, prev_move_type);
            new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//            printf ("-> (2) [%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", i, prev_num_moves, num_moves, prev_move_type, move_type);
//            fflush(stdout);
            move_type = EDLIB_X;

          } else {
            std::vector<int> temp_op(prev_num_moves, EDLIB_X);
            new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//            printf ("-> (3) [%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", i, prev_num_moves, num_moves, prev_move_type, move_type);
//            fflush(stdout);
            num_moves = 0;
            move_type = -1;
          }
        } else {
          std::vector<int> temp_op(prev_num_moves, prev_move_type);
          new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//          printf ("-> (4) [%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", i, prev_num_moves, num_moves, prev_move_type, move_type);
//          fflush(stdout);
        }
      } else {
//        std::vector<int> temp_op(prev_num_moves, prev_move_type);
//        new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//        printf ("-> (4) [%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", i, prev_num_moves, num_moves, prev_move_type, move_type);
//        fflush(stdout);
      }
      prev_move_type = move_type;
      prev_num_moves = num_moves;
      move_type = alignment[i];
      num_moves = 1;
    } else {
      num_moves += 1;
    }
  }
  if (alignmentLength > 0) {
//    std::vector<int> temp_op(prev_num_moves, prev_move_type);
//    new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
    if ((move_type == EDLIB_D && prev_move_type == EDLIB_I) || (move_type == EDLIB_I && prev_move_type == EDLIB_D)) {
      if (prev_num_moves < num_moves) {
        std::vector<int> temp_op(prev_num_moves, EDLIB_X);
        new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//        printf ("-> (1) prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", prev_num_moves, num_moves, prev_move_type, move_type);
//        fflush(stdout);
        num_moves = num_moves - prev_num_moves;
        std::vector<int> temp_op2(num_moves, move_type);
        new_alignment.insert(new_alignment.end(), temp_op2.begin(), temp_op2.end());

      } else if (prev_num_moves > num_moves) {
        std::vector<int> temp_op(prev_num_moves - num_moves, prev_move_type);
        new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//        printf ("-> (2) prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", prev_num_moves, num_moves, prev_move_type, move_type);
//        fflush(stdout);
        move_type = EDLIB_X;
        std::vector<int> temp_op2(num_moves, move_type);
        new_alignment.insert(new_alignment.end(), temp_op2.begin(), temp_op2.end());

      } else {
        std::vector<int> temp_op(prev_num_moves, EDLIB_X);
        new_alignment.insert(new_alignment.end(), temp_op.begin(), temp_op.end());
//        printf ("-> (3) prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", prev_num_moves, num_moves, prev_move_type, move_type);
//        fflush(stdout);
        num_moves = 0;
        move_type = -1;
      }
    } else {
      std::vector<int> temp_op1(prev_num_moves, prev_move_type);
      new_alignment.insert(new_alignment.end(), temp_op1.begin(), temp_op1.end());

      std::vector<int> temp_op2(num_moves, move_type);
      new_alignment.insert(new_alignment.end(), temp_op2.begin(), temp_op2.end());

//      printf ("-> (4) [%ld] prev_num_moves = %ld, num_moves == %ld\tprev_move_type = %d\tmove_type = %d\n", prev_num_moves, num_moves, prev_move_type, move_type);
//      fflush(stdout);
    }

  }

  return new_alignment;
}

int AlignmentToBasicCigar(unsigned char* alignment, int alignmentLength,
                          char** cigar_) {

    unsigned char *alignment_with_clipping = alignment;

    if (alignment[0] == EDLIB_I || alignment[alignmentLength-1] == EDLIB_I) {
      alignment_with_clipping = (unsigned char *) malloc(sizeof(unsigned char) * (alignmentLength + 1));
      memcpy(alignment_with_clipping, alignment, alignmentLength);
      alignment_with_clipping[alignmentLength] = '\0';

      for (int64_t i=0; i<alignmentLength; i++) {
        if (alignment_with_clipping[i] == EDLIB_I)
          alignment_with_clipping[i] = EDLIB_S;
        else break;
      }
      for (int64_t i=(((int64_t) alignmentLength) - 1); i>=0; i--) {
        if (alignment_with_clipping[i] == EDLIB_I)
          alignment_with_clipping[i] = EDLIB_S;
        else break;
      }
    }

    std::vector<char>* cigar = new std::vector<char>();
    unsigned char lastMove = -1;  // Code of last move.
    int numOfSameMoves = 0;
    for (int i = 0; i <= alignmentLength; i++) {
      char alignment_char = 0;
      if (i < alignmentLength) {
        alignment_char = alignment_with_clipping[i];
        /// Replace the 'X' operation with 'M'.
        if (alignment_char == 3)
          alignment_char = 0;
      }

        // if new sequence of same moves started
        if (i == alignmentLength || alignment_char != lastMove) {
            if (i > 0) {  // if previous sequence of same moves ended
                // Write number of moves to cigar string.
                int numDigits = 0;
                for (; numOfSameMoves; numOfSameMoves /= 10) {
                    cigar->push_back('0' + numOfSameMoves % 10);
                    numDigits++;
                }
                reverse(cigar->end() - numDigits, cigar->end());
                // Write code of move to cigar string.
                if (lastMove == 0) {
//                    cigar->push_back('=');
                    cigar->push_back('M');
                } else if (lastMove == 1) {
                    cigar->push_back('I');
                } else if (lastMove == 2) {
                    cigar->push_back('D');
                } else if (lastMove == 3) {
                    cigar->push_back('X');
//                    cigar->push_back('M');
                } else if (lastMove == 4) {
                    cigar->push_back('S');
                } else {
                    delete cigar;
                    return EDLIB_STATUS_ERROR;
                }
            }
            if (i < alignmentLength) {
                numOfSameMoves = 0;
                lastMove = alignment_char;
            }
        }
        numOfSameMoves++;
    }
    cigar->push_back(0);  // Null character termination.
    *cigar_ = (char*) malloc(cigar->size() * sizeof(char));
    // memcpy is not used because it was updated recently and requires new GLIBC symbols,
    // thus binaries won't run on older versions of systems.
    memmove(*cigar_, &(*cigar)[0], cigar->size() * sizeof(char));
    delete cigar;

    if (alignment_with_clipping != alignment)
      free(alignment_with_clipping);

    return EDLIB_STATUS_OK;
}

int AlignmentToExtendedCigar(unsigned char* alignment, int alignmentLength, char** cigar_) {
  std::vector<char>* cigar = new std::vector<char>();

  unsigned char *alignment_with_clipping = alignment;

  if (alignment[0] == EDLIB_I || alignment[alignmentLength-1] == EDLIB_I) {
    alignment_with_clipping = (unsigned char *) malloc(sizeof(unsigned char) * (alignmentLength + 1));
    memcpy(alignment_with_clipping, alignment, alignmentLength);
    alignment_with_clipping[alignmentLength] = '\0';

    for (int64_t i=0; i<alignmentLength; i++) {
      if (alignment_with_clipping[i] == EDLIB_I)
        alignment_with_clipping[i] = EDLIB_S;
      else break;
    }
    for (int64_t i=(((int64_t) alignmentLength) - 1); i>=0; i--) {
      if (alignment_with_clipping[i] == EDLIB_I)
        alignment_with_clipping[i] = EDLIB_S;
      else break;
    }
  }

    unsigned char lastMove = -1;  // Code of last move.
    int numOfSameMoves = 0;
    for (int i = 0; i <= alignmentLength; i++) {
        // if new sequence of same moves started
        if (i == alignmentLength || alignment_with_clipping[i] != lastMove) {
            if (i > 0) {  // if previous sequence of same moves ended
                // Write number of moves to cigar string.
                int numDigits = 0;
                for (; numOfSameMoves; numOfSameMoves /= 10) {
                    cigar->push_back('0' + numOfSameMoves % 10);
                    numDigits++;
                }
                reverse(cigar->end() - numDigits, cigar->end());
                // Write code of move to cigar string.
                if (lastMove == 0) {
                    cigar->push_back('=');
//                    cigar->push_back('M');
               } else if (lastMove == 1) {
                    cigar->push_back('I');
                } else if (lastMove == 2) {
                    cigar->push_back('D');
                } else if (lastMove == 3) {
                    cigar->push_back('X');
//                    cigar->push_back('M');
                } else if (lastMove == 4) {
                    cigar->push_back('S');
                } else {
                    delete cigar;
                    return EDLIB_STATUS_ERROR;
                }
            }
            if (i < alignmentLength) {
                numOfSameMoves = 0;
                lastMove = alignment_with_clipping[i];
            }
        }
        numOfSameMoves++;
    }
    cigar->push_back(0);  // Null character termination.
    *cigar_ = (char*) malloc(cigar->size() * sizeof(char));
    // memcpy is not used because it was updated recently and requires new GLIBC symbols,
    // thus binaries won't run on older versions of systems.
    memmove(*cigar_, &(*cigar)[0], cigar->size() * sizeof(char));
    delete cigar;

    if (alignment_with_clipping != alignment)
      free(alignment_with_clipping);

    return EDLIB_STATUS_OK;
}

int64_t CalculateReconstructedLength(unsigned char *alignment, int alignmentLength) {
  if (alignment == NULL || alignmentLength == 0)
      return 0;

  int64_t length = 0;
  std::stringstream ss;

  for (int i=0; i<alignmentLength; i++) {
    if (alignment[i] == EDLIB_M || alignment[i] == EDLIB_EQUAL || alignment[i] == EDLIB_X || alignment[i] == EDLIB_D)
      length += 1;
  }

  return length;
}

std::string PrintAlignmentToString(const unsigned char* query, const int queryLength,
                    const unsigned char* target, const int targetLength,
                    const unsigned char* alignment, const int alignmentLength,
                    const int position, const int modeCode, int row_width) {
    std::stringstream ss;

    int tIdx = -1;
    int qIdx = -1;
    if (modeCode == EDLIB_MODE_HW) {
        tIdx = position;
        for (int i = 0; i < alignmentLength; i++) {
          if (alignment[i] != EDLIB_I && alignment[i] != EDLIB_S)
                tIdx--;
        }
    }

    for (int start = 0; start < alignmentLength; start += row_width) {
        // target
        ss << "T: ";
        int startTIdx = 0;
        for (int j = start; j < start + row_width && j < alignmentLength; j++) {
            if (alignment[j] == EDLIB_I || alignment[j] == EDLIB_S) {
                ss << "_";

            } else {
                ss << (char) target[++tIdx];
            }
            if (j == start)
                startTIdx = tIdx;
        }
        ss << " (" << std::max(startTIdx, 0) << " - " << tIdx << ")\n";

        // Mismatch
        ss << "   ";
        for (int j = start; j < start + row_width && j < alignmentLength; j++) {
            if (alignment[j] == 0)
              ss << "|";
            else if (alignment[j] == EDLIB_X)
              ss << "X";
            else if (alignment[j] == EDLIB_S)
              ss << "-";
            else
              ss << " ";
        }
        ss << "\n";

        // query
        ss << "Q: ";
        int startQIdx = qIdx;
        for (int j = start; j < start + row_width && j < alignmentLength; j++) {
            if (alignment[j] == EDLIB_D) {
              ss << "_";
            } else {
                ss << (char) query[++qIdx];
            }
            if (j == start)
                startQIdx = qIdx;
        }
        ss << " (" << std::max(startQIdx, 0) << " - " << qIdx << ")\n\n";
    }

    return ss.str();
}

int CountAlignmentOperations(std::vector<unsigned char>& alignment, const int8_t *read_data, const int8_t *ref_data, int64_t reference_hit_id, int64_t alignment_position_start, SeqOrientation orientation,
                             int64_t match, int64_t mismatch, int64_t gap_open, int64_t gap_extend,
                             bool skip_leading_and_trailing_insertions,
                             int64_t* ret_eq, int64_t* ret_x, int64_t* ret_i, int64_t* ret_d, int64_t *ret_alignment_score, int64_t *ret_edit_dist, int64_t *ret_nonclipped_length) {
  unsigned char last_move = -1;  // Code of last move.
  int64_t num_same_moves = 0;
  int64_t read_position = 0;
  int64_t ref_position = 0;

  int64_t num_eq = 0;
  int64_t num_x = 0;
  int64_t num_i = 0;
  int64_t num_d = 0;
  int64_t alignment_score = 0;

  int64_t nonclipped_length = 0;

  int64_t start_op = 0, end_op = alignment.size() - 1;
  if (skip_leading_and_trailing_insertions == true) {
    for (start_op = 0; start_op < alignment.size(); start_op++, read_position++) {
      if (alignment[start_op] != EDLIB_I && alignment[start_op] != EDLIB_S) { break; }
    }
    for (end_op = (alignment.size() - 1); end_op >= 0; end_op--) {
      if (alignment[end_op] != EDLIB_I && alignment[start_op] != EDLIB_S) { break; }
    }
  }

  for (int i = start_op; i <= end_op; i++) {
    char align_op = 255;
    align_op = alignment[i];

    if (align_op == EDLIB_M || align_op == EDLIB_EQUAL || align_op == EDLIB_X) {
      if (read_data[read_position] == ref_data[alignment_position_start + ref_position]) {
        num_eq += 1;
        alignment_score += match;

      } else {
        num_x += 1;
        alignment_score -= mismatch;
      }

      nonclipped_length += 1;

    } else if (align_op == EDLIB_I) {
      num_i += 1;
      /// This is for the (gap_open + (N) * gap_extend).
      ///      alignment_score -= ((i == 0 || (i > 0 && alignment[i-1] != align_op)) ? (gap_open + gap_extend) : gap_extend);
      /// This is for the (gap_open + (N - 1) * gap_extend).
      alignment_score -= ((i == 0 || (i > 0 && alignment[i-1] != align_op)) ? (gap_open) : gap_extend);
      nonclipped_length += 1;

    } else if (align_op == EDLIB_D) {
      num_d += 1;
      /// This is for the (gap_open + (N) * gap_extend).
      ///      alignment_score -= ((i == 0 || (i > 0 && alignment[i-1] != align_op)) ? (gap_open + gap_extend) : gap_extend);
      /// This is for the (gap_open + (N - 1) * gap_extend).
      alignment_score -= ((i == 0 || (i > 0 && alignment[i-1] != align_op)) ? (gap_open) : gap_extend);
    }

    // Increase coordinates.
    if (align_op == EDLIB_M || align_op == EDLIB_EQUAL || align_op == EDLIB_X || align_op == EDLIB_I || align_op == EDLIB_S)
      read_position += 1;
    if (align_op == EDLIB_M || align_op == EDLIB_EQUAL || align_op == EDLIB_X || align_op == EDLIB_D)
      ref_position += 1;
  }

  *ret_eq = num_eq;
  *ret_x = num_x;
  *ret_i = num_i;
  *ret_d = num_d;
  *ret_alignment_score = alignment_score;
  *ret_nonclipped_length = nonclipped_length;
  *ret_edit_dist = num_x + num_i + num_d;

  return 0;
}

std::string AlignmentToMD(std::vector<unsigned char>& alignment, const int8_t *read_data, const int8_t *ref_data, int64_t reference_hit_id, int64_t alignment_position_start) {
  int64_t num_same_moves = 0;
  int64_t read_position = 0;
  int64_t ref_position = 0;

  int64_t num_eq = 0;
  int64_t num_d = 0;

  std::stringstream md;

  for (int i = 0; i < alignment.size(); i++) {
    char align_op = alignment[i];

    if (align_op == EDLIB_S) {
      num_eq = 0;
      num_d = 0;
      continue;
    }

    if (align_op == EDLIB_M || align_op == EDLIB_EQUAL) {
      num_eq += 1;
      num_d = 0;
      ref_position += 1;

    } else if (align_op == EDLIB_I) {
      continue;

    } else {
//      if (num_eq > 0) {
        if(!(align_op == EDLIB_D && num_d != 0)){
          md << num_eq; num_eq = 0;
        }
//      }
      if (align_op == EDLIB_X) {
        md << ((char) ref_data[alignment_position_start + ref_position]);
        num_eq = 0;
        num_d = 0;
        ref_position += 1;
      } else if (align_op == EDLIB_D) {
        if (num_d == 0) { md << "^"; }
        md << ((char) ref_data[alignment_position_start + ref_position]);
        num_eq = 0;
        num_d += 1;
        ref_position += 1;
      } else if (align_op == EDLIB_I) {
        num_d = 0;
        num_eq = 0;
      }
    }
  }

  if (num_eq > 0) {
    md << num_eq; num_eq = 0;
    num_eq = 0;
  }

  return md.str();
}

int GetAlignmentPatterns(const unsigned char* query, const int64_t queryLength,
                         const unsigned char* target, const int64_t targetLength,
                         const unsigned char* alignment, const int64_t alignmentLength,
                         std::string& ret_query, std::string& ret_target, std::string& ret_match_pattern) {
  std::stringstream ss_query, ss_target, ss_pattern;
  int64_t qpos = 0, tpos = 0;
  for (int64_t i=0; i<alignmentLength; i++) {
    if (alignment[i] == EDLIB_H) {
      continue;

    } else if (alignment[i] == EDLIB_S) {
      qpos += 1;
      continue;

    } else if (alignment[i] == EDLIB_M || alignment[i] == EDLIB_EQUAL || alignment[i] == EDLIB_X) {
      ss_query << query[qpos];
      ss_target << target[tpos];
      ss_pattern << ((alignment[i] == EDLIB_X) ? "*" : "|");
      qpos += 1;
      tpos += 1;

    } else if (alignment[i] == EDLIB_I) {
      ss_query << query[qpos];
      ss_target << "-";
      ss_pattern << "*";
      qpos += 1;

    } else if (alignment[i] == EDLIB_D) {
      ss_query << "-";
      ss_target << target[tpos];
      ss_pattern << "*";
      tpos += 1;

    } else {
      fprintf (stderr, "ERROR: Unknown EDLIB operation %d!\n", alignment[i]);
    }
  }

  ret_query = ss_query.str();
  ret_target = ss_target.str();
  ret_match_pattern = ss_pattern.str();

  return 0;
}
